#ifndef ADVANCE_PARSER_H
#define ADVANCE_PARSER_H

#include <string>

namespace Project::Utilities {
  class AdvanceParser {
  public:
    static int parseInt(const std::string& value);
    static float parseFloat(const std::string& value);
    static void parseSpacing(const std::string& str, int& top, int& right, int& bottom, int& left);
  };
}

#endif
