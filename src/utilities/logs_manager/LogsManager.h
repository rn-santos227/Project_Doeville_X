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

  void logError(const std::string& message);
  void logMessage(const std::string& message);

  template <typename T>
  T checkAndLogError(bool condition, const std::string& errorMsg, T returnValue) {
    if (condition) {
      logError(errorMsg + " SDL_Error: " + SDL_GetError());
      return returnValue;
    }
    return T();
  }

private:
  std::ofstream logFile;

  std::string getCurrentTimestamp();
};

#endif