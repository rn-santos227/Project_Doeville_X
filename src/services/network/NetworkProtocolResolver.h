#ifndef NETWORK_PROTOCOL_RESOLVER_H
#define NETWORK_PROTOCOL_RESOLVER_H

#include "NetworkProtocol.h"
#include <string_view>

namespace Project::Services {
  class NetworkProtocolResolver {
  public:
    static NetworkProtocol resolve(std::string_view name);
  };
}

#endif
