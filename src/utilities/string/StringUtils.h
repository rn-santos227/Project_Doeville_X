#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <string_view>

namespace Project::Utilities {
  class StringUtils {
  public:
    static std::string to_lower(std::string_view input);
    static std::string to_upper(std::string_view input);
    static bool iequals(std::string_view a, std::string_view b);
  };
}

#endif 
