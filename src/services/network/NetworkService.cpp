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

  void NetworkService::asyncGet(const std::string& endpoint, ResponseHandler handler, const std::vector<std::pair<std::string, std::string>>& headers, const std::string& tokenKey) {
    std::thread([this, endpoint, handler = std::move(handler), tokenKey, headers]() {
      logsManager.logMessage(std::string(Project::Libraries::Constants::GET_METHOD) + " " + endpoint);
      auto header = constructHeader(headers, tokenKey);
      header.build();
      Payload payload(endpoint.begin(), endpoint.end());
      std::lock_guard<std::mutex> lock(responseMutex);
      responses.push({handler, std::move(payload)});
    }).detach();
  }

  void NetworkService::asyncPost(const std::string& endpoint, const Payload& payload, ResponseHandler handler, const std::vector<std::pair<std::string, std::string>>& headers, const std::string& tokenKey) {
    std::thread([this, endpoint, payload, handler = std::move(handler), tokenKey, headers]() mutable {
      logsManager.logMessage(std::string(Project::Libraries::Constants::POST_METHOD) + " " + endpoint);
      auto header = constructHeader(headers, tokenKey);
      header.build();
      Payload reply = payload;
      std::lock_guard<std::mutex> lock(responseMutex);
      responses.push({handler, std::move(reply)});
    }).detach();
  }

  HttpHeader NetworkService::constructHeader(const std::vector<std::pair<std::string, std::string>>& headers, const std::string& tokenKey) {
    HttpHeader header;
    for (const auto& kv : headers) header.add(kv.first, kv.second);
    if (!tokenKey.empty()) {
    auto token = getToken(tokenKey);
    if (!token.empty()) {
      header.add(Project::Libraries::Constants::HEADER_AUTH, std::string(Project::Libraries::Constants::HEADER_BEARER) + token);
      logsManager.logMessage("Using token for header: " + tokenKey);
    } else {
      logsManager.logWarning("Auth token missing for key: " + tokenKey);
    }
    }
    return header;
  }

  void NetworkService::poll() {
    while (!inbound.empty()) {
      auto msg = std::move(inbound.front());
      for (auto& handler : messageHandlers)
        if (handler) handler(msg);
      inbound.pop();
    }

    std::lock_guard<std::mutex> lock(responseMutex);
  }

  void NetworkService::cacheToken(const std::string& key, const std::string& token) {
    tokenCache[key] = token;
  }

  std::string NetworkService::getToken(const std::string& key) {
    auto it = tokenCache.find(key);
    if (it != tokenCache.end()) return it->second;

    auto token = envLoader.getValue(key, Project::Libraries::Constants::EMPTY_STRING);
    if (!token.empty()) tokenCache[key] = token;
    return token;
  }
}
