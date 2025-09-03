#include "NetworkService.h"
#include "HttpHeader.h"

#include <thread>
#include <utility>

namespace Project::Services {
  using Project::Utilities::LogsManager;

  NetworkService::NetworkService(LogsManager& logs, const std::string& envPath)
    : logsManager(logs), envLoader(logsManager, envPath) {
    envLoader.loadEnv();
  }

  bool NetworkService::connect(const std::string& endpoint, Protocol protocol, const std::string& tokenKey) {
    logsManager.logMessage("Connecting to " + endpoint);
    if (!tokenKey.empty()) {
      auto token = getToken(tokenKey);
      if (token.empty())
        logsManager.logWarning("Auth token missing for key: " + tokenKey);
      else
        logsManager.logMessage("Auth token loaded for key: " + tokenKey);
    }
    return true;
  }

  std::string NetworkService::getToken(const std::string& key) {
    auto it = tokenCache.find(key);
    if (it != tokenCache.end()) return it->second;

    auto token = envLoader.getValue(key, Project::Libraries::Constants::EMPTY_STRING);
    if (!token.empty()) tokenCache[key] = token;
    return token;
  }
}
