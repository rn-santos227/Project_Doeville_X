#ifndef NETWORK_DATA_H
#define NETWORK_DATA_H

#include <string>

namespace Project::Components {
  struct NetworkData {
    std::string endpoint;
    std::string protocol;
    std::string tokenKey;
    bool connected{false};
  };
}

#endif
