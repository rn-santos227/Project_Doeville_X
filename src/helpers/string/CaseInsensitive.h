#ifndef CASE_INSENSITIVE_H
#define CASE_INSENSITIVE_H

#include <string_view>
#include <cctype>

namespace Project::Helpers {
  inline bool iequals(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
      if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) {
        return false;
      }
    }
    return true;
  }
}

#endif
