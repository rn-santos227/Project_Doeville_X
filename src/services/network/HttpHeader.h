#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <string>
#include <vector>
#include <utility>

namespace Project::Services {
  struct HttpHeader {
    std::vector<std::pair<std::string, std::string>> fields;
  };
}

#endif
