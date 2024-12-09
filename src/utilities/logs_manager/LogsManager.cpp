#include "LogsManager.h"

LogsManager::LogsManager(const std::string& logFilePath) : logFilePath(logFilePath) {
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

void LogsManager::logError(const std::string& message) {
  std::string timestamp = getCurrentTimestamp();
  std::string logMessage = "[ERROR] " + timestamp + " - " + message + "\n";

  std::cerr << logMessage;

  if (logFile.is_open()) {
    logFile << logMessage;
  }
}

void LogsManager::logMessage(const std::string& message) {
  std::string timestamp = getCurrentTimestamp();
  std::string logMessage = "[INFO] " + timestamp + " - " + message + "\n";

  std::cout << logMessage;

  if (logFile.is_open()) {
    logFile << logMessage;
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