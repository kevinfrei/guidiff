#pragma once

#include <string>

#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/websocket.h>

namespace handlers {

crow::response www_path(const crow::request& req, const std::string& path);
crow::response left(const crow::request&, const std::string& path);
crow::response right(const crow::request& req, const std::string& path);
crow::response api(const crow::request& req, const std::string& path);
crow::response keepalive();
crow::response quit();
void socket_message(crow::websocket::connection& conn,
                    const std::string& data,
                    bool is_binary);

} // namespace handlers
