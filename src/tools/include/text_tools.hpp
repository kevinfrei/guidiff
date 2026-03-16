#pragma once

#include <cctype>
#include <charconv>
#include <cwctype>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "json_pickling.hpp"

namespace text {

// Helper to handle both char and wchar_t lowering
template <typename T>
T toggle_lower(T c) {
  if constexpr (std::is_same_v<T, wchar_t>) {
    return std::towlower(c);
  } else {
    return static_cast<T>(std::tolower(static_cast<unsigned char>(c)));
  }
}

template <typename T>
std::enable_if_t<!is_enum_class_v<T>, std::optional<T>> to_integer(
    std::string_view sv) {
  T value;
  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (ec == std::errc{} && ptr == sv.data() + sv.size()) {
    return value;
  }
  return std::nullopt;
}

template <typename T>
inline std::enable_if_t<is_enum_class_v<T>, std::optional<T>> to_integer(
    std::string_view value) {
  auto res = to_integer<std::underlying_type_t<T>>(value);
  if (res) {
    return static_cast<T>(*res);
  } else {
    return std::nullopt;
  }
}

std::string lowercase(std::string_view str);
bool iequals(std::string_view lhs, std::string_view rhs);

} // namespace text