#ifndef PROTOCOL_CATEGORIES_H
#define PROTOCOL_CATEGORIES_H

namespace Project::Libraries::Categories {
  namespace Protocols {
    constexpr size_t PROTOCOL_NAME_ALIAS_COUNT = 3;

    constexpr const char* HTTP = "HTTP";
    constexpr const char* WEBSOCKET = "WEBSOCKET";
    constexpr const char* P2P = "P2P";
  }
}

#endif
