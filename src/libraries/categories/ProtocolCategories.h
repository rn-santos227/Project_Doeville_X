#ifndef PROTOCOL_CATEGORIES_H
#define PROTOCOL_CATEGORIES_H

namespace Project::Libraries::Categories {
  namespace Protocols {
    constexpr size_t PROTOCOL_NAME_ALIAS_COUNT = 3;

    constexpr const char* HTTP = "http";
    constexpr const char* WEBSOCKET = "websocket";
    constexpr const char* P2P = "p2p";
  }
}

#endif
