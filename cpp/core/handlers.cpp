#include <cctype>
#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include <crow.h>

#include "CommonTypes.hpp"
#include "api.hpp"
#include "config.hpp"
#include "files.hpp"
#include "quitting.hpp"
#include "setup.hpp"
#include "text_tools.hpp"
#include "tools.hpp"

#include "handlers.hpp"

namespace handlers {

crow::response www_path(const crow::request&, const std::string& path) {
  quitting::keep_alive();

  CROW_LOG_INFO << "Path: " << path;
  crow::response resp;
  std::filesystem::path p =
      files::get_web_dir() / (path.empty() ? "index.html" : path);
  if (p.filename() == "index.html") {
    // If we're sending index.html, we should *clear* ready
    config::not_ready();

    // We need to process the index.html file to replace the websocket URL
    // with the correct one.

    // Read the contents of the index.html file,
    // Replace "window.wsport = 42;" with the actual port number
    std::ifstream file(p);
    if (!file.is_open()) {
      tools::e404(resp, "index.html not found");
      return resp;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    // Replace the placeholder with the actual port number
    std::string wsport = std::to_string(setup::get_random_port());
    size_t pos = content.find("window.wsport = 42;");
    if (pos != std::string::npos) {
      content.replace(pos, 20, "window.wsport = " + wsport + ";");
      resp.body = content;
      resp.code = 200;
      resp.set_header("Content-Type", "text/html");
    } else {
      CROW_LOG_ERROR << "Placeholder not found in index.html file: "
                     << p.generic_string();
      resp.code = 500;
      resp.body = "Internal Server Error";
      resp.set_header("Content-Type", "text/plain");
    }
  } else {
    resp.set_static_file_info_unsafe(p.generic_string());
    resp.set_header("Content-type", files::path_to_mime_type(p));
  }
  return resp;
}

crow::response api(const crow::request&, const std::string& path) {
  quitting::keep_alive();

  CROW_LOG_DEBUG << "API Path: " << path;
  crow::response resp;
  size_t slashPos = path.find('/');
  slashPos = (slashPos == path.npos) ? path.size() : slashPos;
  auto maybeCall = text::to_integer<Shared::IpcCall>(
      std::string_view{path.c_str(), slashPos});
  if (!maybeCall) {
    tools::e404(resp, "Invalid API arguments for path " + path);
    return resp;
  }
  if (!Shared::is_valid(*maybeCall)) {
    tools::e404(resp, "Unknown API for path " + path);
    return resp;
  }
  auto ValidateAndCall =
      [&](std::function<void(crow::response&, std::string_view)> handle_call,
          bool decode) -> void {
    if (slashPos == path.size()) {
      tools::e404(resp, "No data provided for API" + path);
    } else {
      const std::string_view data{path.c_str() + slashPos + 1};
      if (decode) {
        auto maybeDecoded = tools::url_decode(data);
        if (!maybeDecoded) {
          tools::e404(resp, "Invalid URL encoding for API " + path + ":");
          CROW_LOG_ERROR << data;
        } else {
          resp.code = 200;
          handle_call(resp, *maybeDecoded);
        }
      } else {
        handle_call(resp, data);
      }
    }
  };
  switch (*maybeCall) {
    case Shared::IpcCall::WriteToStorage:
      ValidateAndCall(api::write_to_storage, false);
      break;
    case Shared::IpcCall::ReadFromStorage:
      ValidateAndCall(api::read_from_storage, true);
      break;
    case Shared::IpcCall::DeleteFromStorage:
      ValidateAndCall(api::delete_from_storage, true);
      break;
    case Shared::IpcCall::ShowOpenDialog:
      ValidateAndCall(files::folder_picker, true);
      break;
    default:
      if (Shared::is_valid(*maybeCall)) {
        CROW_LOG_ERROR << "Unimplemented API call received: "
                       << Shared::to_string(*maybeCall) << " ("
                       << underlying_cast(*maybeCall) << ") [" << path << "]";
      } else {
        CROW_LOG_ERROR << "Unknown API call received: "
                       << underlying_cast(*maybeCall) << " [" << path << "]";
      }
      std::string error_message =
          Shared::is_valid(*maybeCall)
              ? "Unimplemented API call: " +
                    std::string(Shared::to_string(*maybeCall))
              : "Unknown API call: " +
                    std::to_string(underlying_cast(*maybeCall));
      tools::e404(resp, error_message);
      return resp;
  }

  return resp;
}

crow::response left(const crow::request&, const std::string& path) {
  quitting::keep_alive();

  CROW_LOG_INFO << "left: " << path;
  crow::response resp;
  std::filesystem::path p =
      files::get_web_dir() / (path.empty() ? "index.html" : path);
  resp.body = path;
  resp.code = 200;
  resp.set_header("Content-type", files::path_to_mime_type(p));
  return resp;
}

crow::response right(const crow::request&, const std::string& path) {
  quitting::keep_alive();

  CROW_LOG_INFO << "left: " << path;
  crow::response resp;
  std::filesystem::path p =
      files::get_web_dir() / (path.empty() ? "index.html" : path);
  resp.body = path;
  resp.code = 200;
  resp.set_header("Content-type", files::path_to_mime_type(p));
  return resp;
}

crow::response keepalive() {
  quitting::keep_alive();
  crow::response resp;
  resp.code = 200;
  resp.body = "OK";
  resp.set_header("Content-Type", "text/plain");
  return resp;
}

crow::response quit() {
  quitting::really_quit();
  return crow::response(200);
}

void socket_message(crow::websocket::connection& conn,
                    const std::string& data,
                    bool /* is_binary */) {
  CROW_LOG_INFO << "Got a message from the client:" << data;
  // Message is IpcMessage;[json-formatted array of arguments]
  size_t pos = data.find(';');
  if (pos == data.npos) {
    CROW_LOG_ERROR << "Invalid websocket message received: " << data;
    return;
  }
  auto maybeMsg =
      text::to_integer<Shared::SocketMsg>(std::string_view{data.c_str(), pos});
  if (!maybeMsg) {
    CROW_LOG_ERROR << "Invalid websocket message received: " << data;
    return;
  }
  Shared::SocketMsg msg = *maybeMsg;
  if (!Shared::is_valid(msg)) {
    CROW_LOG_ERROR << "Invalid Socket message received: " << data;
    return;
  }
  // This is the only message we support *receiving* from the client
  switch (msg) {
    case Shared::SocketMsg::ContentLoaded:
      CROW_LOG_INFO << "Client finished loading content.\n"
                    << "marking config as ready and sending music db";
      config::set_ready();
      break;
    default: // Unsupported message
      CROW_LOG_ERROR
          << "Unsupported message received: " << Shared::to_string(msg) << " ("
          << underlying_cast(msg) << ") [" << data << "]";
  }
}

} // namespace handlers