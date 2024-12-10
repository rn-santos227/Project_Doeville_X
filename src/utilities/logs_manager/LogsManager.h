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
  LogsManager();
  ~LogsManager();

  bool checkAndLogError(bool condition, const std::string& errorMsg);

  void flushLogs();
  void logError(const std::string& message);
  void logMessage(const std::string& message);

private:
  std::ofstream logFile;

  std::string getCurrentTimestamp();
};

#endif