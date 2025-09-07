#ifndef NETWORK_CONSTANTS_H
#define NETWORK_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr const char* GET_METHOD = "GET";
  constexpr const char* POST_METHOD = "POST";
  constexpr const char* HEADER_AUTH = "Authorization";
  constexpr const char* HEADER_BEARER = "Bearer ";

  constexpr const char* HTTP = "http";
  constexpr const char* P2P = "p2p";
  constexpr const char* WEBSOCKET = "websocket";
}

#endif
