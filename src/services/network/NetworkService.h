#ifndef NETWORK_SERVICE_H
#define NETWORK_SERVICE_H

#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>

#include "libraries/constants/ScriptConstants.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/env_loader/EnvLoader.h"

namespace Project::Services {
  class NetworkService {
  public:
    enum class Protocol { HTTP, WebSocket, P2P };
    using Payload = std::vector<std::uint8_t>;
    
    explicit NetworkService(Project::Utilities::LogsManager& logs, const std::string& envPath = Project::Libraries::Constants::ENV_EXTENSION);

  private:
    struct PendingResponse {

    };
  };
}

#endif
