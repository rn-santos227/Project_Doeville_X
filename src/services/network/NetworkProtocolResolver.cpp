#include "NetworkProtocolResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/ProtocolCategories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Services {
  namespace Protocols = Project::Libraries::Categories::Protocols;

  NetworkProtocol NetworkProtocolResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, NetworkProtocol>, Protocols::PROTOCOL_NAME_ALIAS_COUNT> map{{
      {Protocols::HTTP, NetworkProtocol::HTTP},
      {Protocols::WEBSOCKET, NetworkProtocol::WebSocket},
      {Protocols::P2P, NetworkProtocol::P2P},
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return NetworkProtocol::HTTP;
  }
}
