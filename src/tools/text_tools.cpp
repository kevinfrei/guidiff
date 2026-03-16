#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <string_view>

#include "text_tools.hpp"

namespace text {

// Just lowercase an ASCII string
std::string lowercase(std::string_view str) {
  std::string result;
  result.reserve(str.size());
  for (char c : str) {
    result += toggle_lower(c);
  }
  return result;
}

bool iequals(std::string_view lhs, std::string_view rhs) {
  return std::ranges::equal(lhs, rhs, [](unsigned char a, unsigned char b) {
    return std::tolower(a) == std::tolower(b);
  });
}

} // namespace text