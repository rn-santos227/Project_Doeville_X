#include "LogsManager.h"

namespace Project::Utilities {
  LogsManager::LogsManager() {
    std::string logFilePath = "resources/logs/logs.txt";
    std::filesystem::create_directories("resources/logs");
    
    logFile.open(logFilePath, std::ios_base::app);
    
    if (!logFile.is_open()) {
      logFile.open(logFilePath, std::ios_base::out);
      if (!logFile.is_open()) {
        std::cerr << "Failed to create log file: " << logFilePath << std::endl;
      }
    }
  }

  LogsManager::~LogsManager() {
    if (logFile.is_open()) {
      logFile.close();
    };
  }

  bool LogsManager::checkAndLogError(bool condition, const std::string& errorMsg) {
    if(condition) {
      logError(errorMsg + " SDL_Error: " + SDL_GetError());
    }
    return condition;
  }

  void LogsManager::logError(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::string sanitizedMessage = sanitizePath(message);
    
    std::string logMessage = "[ERROR] " + timestamp + " - " + sanitizedMessage + "\n";

    std::cerr << logMessage;

    if (logFile.is_open()) {
      logFile << logMessage;
    }
  }

  void LogsManager::logMessage(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::string sanitizedMessage = sanitizePath(message);

    std::string logMessage = "[INFO] " + timestamp + " - " + sanitizedMessage + "\n";

    std::cout << logMessage;

    if (logFile.is_open()) {
      logFile << logMessage;
    }
  }

  void LogsManager::flushLogs() {
    if (logFile.is_open()) {
      logFile.flush();
      logFile.clear();
    }
  }


  std::string LogsManager::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream timestampStream;
    timestampStream << (localTime->tm_year + 1900) << "-" 
                    << (localTime->tm_mon + 1) << "-"
                    << localTime->tm_mday << " "
                    << localTime->tm_hour << ":"
                    << localTime->tm_min << ":"
                    << localTime->tm_sec;

    return timestampStream.str();
  }

  std::string LogsManager::sanitizePath(const std::string& path) {
    std::string sanitized = path;
    std::replace(sanitized.begin(), sanitized.end(), '\\', '/');
    return sanitized;
  }
}