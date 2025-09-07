#ifndef NETWORK_SERVICE_H
#define NETWORK_SERVICE_H

#include "HttpHeader.h"
#include "NetworkProtocol.h"

#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>

#include "libraries/constants/NameConstants.h"
#include "libraries/constants/ScriptConstants.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/env_loader/EnvLoader.h"

namespace Project::Services {
  class NetworkService {
  public:
    explicit NetworkService(Project::Utilities::LogsManager& logs, const std::string& envPath = Project::Libraries::Constants::ENV_EXTENSION);

    using Payload = std::vector<std::uint8_t>;
    using ResponseHandler = std::function<void(const Payload&)>;
    using MessageHandler = std::function<void(const Payload&)>;

    bool connect(const std::string& endpoint, NetworkProtocol protocol, const std::string& tokenKey = Project::Libraries::Constants::EMPTY_STRING);
    void send(const Payload& data);
    void onPayload(MessageHandler handler);

    void asyncGet(
      const std::string& endpoint,
      ResponseHandler handler,
      const std::vector<std::pair<std::string, std::string>>& headers = {},
      const std::string& tokenKey = Project::Libraries::Constants::EMPTY_STRING
    );

    void asyncPost(
      const std::string& endpoint,
      const Payload& payload,
      ResponseHandler handler,
      const std::vector<std::pair<std::string, std::string>>& headers = {},
      const std::string& tokenKey = Project::Libraries::Constants::EMPTY_STRING
    );

    HttpHeader constructHeader(
      const std::vector<std::pair<std::string,std::string>>& headers = {},
      const std::string& tokenKey = Project::Libraries::Constants::EMPTY_STRING
    );

    void poll();
    void cacheToken(const std::string& key, const std::string& token);
    std::string getToken(const std::string& key);
  
  private:
    struct PendingResponse {
      ResponseHandler handler;
      Payload payload;
    };

    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::EnvLoader envLoader;

    std::unordered_map<std::string, std::string> tokenCache;
    std::vector<MessageHandler> messageHandlers;
    std::queue<Payload> inbound;

    std::mutex responseMutex;
    std::queue<PendingResponse> responses;
  };
}

#endif
