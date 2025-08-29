#include "AdvanceParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace Project::Utilities {
  int AdvanceParser::parseInt(const std::string& value) {
    std::string v = value;
    v.erase(std::remove_if(v.begin(), v.end(), [](char c){
      return !std::isdigit(static_cast<unsigned char>(c)) && c != '-';
    }), v.end());
    if (v.empty()) return 0;
    return std::stoi(v);
  }

  float AdvanceParser::parseFloat(const std::string& value) {
    std::string v = value;
    v.erase(std::remove_if(v.begin(), v.end(), [](char c){
      return !(std::isdigit(static_cast<unsigned char>(c)) || c == '.' || c == '-');
    }), v.end());
    if (v.empty()) return 0.0f;
    return std::stof(v);
  }

  void AdvanceParser::parseSpacing(const std::string& str, int& top, int& right,
                                   int& bottom, int& left) {
    std::istringstream iss(str);
    std::vector<int> vals;
    std::string token;
    while (iss >> token) {
      vals.push_back(parseInt(token));
    }
    if (vals.size() == 1) {
      top = right = bottom = left = vals[0];
    } else if (vals.size() == 2) {
      top = bottom = vals[0];
      right = left = vals[1];
    } else if (vals.size() == 3) {
      top = vals[0];
      right = left = vals[1];
      bottom = vals[2];
    } else if (vals.size() >= 4) {
      top = vals[0];
      right = vals[1];
      bottom = vals[2];
      left = vals[3];
    }
  }
}

