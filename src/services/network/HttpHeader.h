#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <string>
#include <vector>
#include <utility>

namespace Project::Services {
  struct HttpHeader {
    std::vector<std::pair<std::string, std::string>> fields;

    void add(const std::string& key, const std::string& value) {
      fields.emplace_back(key, value);
    }

    std::string build() const {

    }
  };
}

#endif
