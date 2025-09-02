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
    explicit NetworkService(Project::Utilities::LogsManager& logs, const std::string& envPath = Project::Libraries::Constants::ENV_EXTENSION);

    using Payload = std::vector<std::uint8_t>;
    using ResponseHandler = std::function<void(const Payload&)>;
    using MessageHandler = std::function<void(const Payload&)>;

  private:
    struct PendingResponse {
      ResponseHandler handler;
      Payload payload;
    };

    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::EnvLoader envLoader;

    std::unordered_map<std::string, std::string> tokenCache;
  };
}

#endif
