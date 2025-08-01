#include "StringUtils.h"

#include <cctype>

namespace Project::Utilities {
  std::string StringUtils::to_lower(std::string_view input) {
    std::string result(input);
    for (auto& ch : result) {
      ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
  }

  std::string StringUtils::to_upper(std::string_view input) {
    std::string result(input);
    for (auto& ch : result) {
      ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return result;
  }

  bool StringUtils::iequals(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
      if (std::tolower(static_cast<unsigned char>(a[i])) !=
          std::tolower(static_cast<unsigned char>(b[i]))) {
        return false;
      }
    }
    return true;
  }
}
