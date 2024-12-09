#ifndef LOGS_MANAGER_H
#define LOGS_MANAGER_H

#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>

class LogsManager {
public:
  LogsManager(const std::string& logFilePath);
  ~LogsManager();

  void logError(const std::string& message);
  void logMessage(const std::string& message);

  template <typename T>
  T checkAndLogError(bool condition, const std::string& errorMsg, T returnValue) {
    if (condition) {
      return returnValue;
    }
  }

private:
  std::ofstream logFile;
  std::string logFilePath;

  std::string getCurrentTimestamp();
};

#endif