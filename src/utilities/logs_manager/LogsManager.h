#ifndef LOGS_MANAGER_H
#define LOGS_MANAGER_H

#include <algorithm> 
#include <iostream>
#include <filesystem> 
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>

#include <SDL.h>
namespace Project::Utilities {
  class LogsManager {
  public:
    LogsManager();
    ~LogsManager();

    bool checkAndLogError(bool condition, const std::string& errorMsg);

    void flushLogs();
    
    void logError(const std::string& message);
    void logLuaMessage(const std::string& message);
    void logMessage(const std::string& message);
    void logWarning(const std::string& message);

  private:
    std::ofstream logFile;

    std::string getCurrentTimestamp();
    std::string sanitizePath(const std::string& path);
  };
}

#endif