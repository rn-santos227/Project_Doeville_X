#ifndef LOGS_MANAGER_H
#define LOGS_MANAGER_H

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <ctime>
#include <filesystem> 
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

#include <SDL.h>
namespace Project::Utilities {
  struct LogEntry {
    std::string message;
    std::ostream* stream;
    std::ofstream* file;
  };

  class LogsManager {
  public:
    LogsManager();
    ~LogsManager();

    bool checkAndLogError(bool condition, const std::string& errorMsg);

    void enqueueLog(const std::string& message, std::ostream* stream, std::ofstream* file);
    void flushLogs();
    
    void logError(const std::string& message);
    void logLuaMessage(const std::string& message);
    void logMessage(const std::string& message);
    void logWarning(const std::string& message);

    const std::string& getLogFilePath() const;
    const std::string& getLuaLogFilePath() const;

    void showErrorPopup(const std::string& message);
    void showWarningPopup(const std::string& message);
    void printConsoleOnly(const std::string& message);

  private:
    std::ofstream logFile;
    std::ofstream luaLogFile;
    std::string logFilePath;
    std::string luaLogFilePath;


    std::string getCurrentTimestamp();
    std::string sanitizePath(const std::string& path);
  };
}

#endif
