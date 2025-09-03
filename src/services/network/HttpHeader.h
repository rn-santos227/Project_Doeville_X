#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <string>
#include <vector>
#include <utility>

#include "libraries/constants/RegexConstants.h"

namespace Project::Services {
  struct HttpHeader {
    std::vector<std::pair<std::string, std::string>> fields;

    void add(const std::string& key, const std::string& value) {
      fields.emplace_back(key, value);
    }

    std::string build() const {
      std::string result;
      for (const auto& kv : fields) {
        result += kv.first + ": " + kv.second + Project::Libraries::Constants::REGEX_HTTP_HEADER_BREAKER;
      }
      result += Project::Libraries::Constants::REGEX_HTTP_HEADER_BREAKER;
      return result;
    }
  };
}

#endif
