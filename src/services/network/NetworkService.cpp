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

    }
    return true;
  }
}
