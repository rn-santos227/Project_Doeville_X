#ifndef NETWORK_DATA_H
#define NETWORK_DATA_H

#include <string>

#include "services/network/NetworkProtocol.h"

namespace Project::Components {
  struct NetworkData {
    Project::Services::NetworkProtocol protocol{
      Project::Services::NetworkProtocol::HTTP
    };
    
    std::string endpoint;
    std::string tokenKey;
    bool connected{false};
  };
}

#endif
