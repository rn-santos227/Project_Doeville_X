#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <cctype>
#include <string>
#include <string_view>

namespace Project::Utilities {
  class StringUtils {
  public:
    static inline bool iequals(std::string_view a, std::string_view b);
    static inline std::string to_lower(std::string_view input);
    static inline std::string to_upper(std::string_view input);
  };
}

#endif 
