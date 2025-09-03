#include "NetworkService.h"
#include "HttpHeader.h"

#include <string>
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

  void NetworkService::send(const Payload& data) { inbound.push(data); }

  void NetworkService::onPayload(MessageHandler handler) {
    messageHandlers.push_back(std::move(handler));
  }

  void NetworkService::cacheToken(const std::string& key, const std::string& token) {
    tokenCache[key] = token;
  }

  void NetworkService::asyncGet(const std::string& endpoint, ResponseHandler handler, const std::string& tokenKey) {
      std::thread([this, endpoint, handler = std::move(handler), tokenKey]() {
      auto token = tokenKey.empty() ? std::string() : getToken(tokenKey);
      logsManager.logMessage(std::string(Project::Libraries::Constants::GET_METHOD) + " " + endpoint);
      HttpHeader header;
    }).detach();
  }

  std::string NetworkService::getToken(const std::string& key) {
    auto it = tokenCache.find(key);
    if (it != tokenCache.end()) return it->second;

    auto token = envLoader.getValue(key, Project::Libraries::Constants::EMPTY_STRING);
    if (!token.empty()) tokenCache[key] = token;
    return token;
  }
}
