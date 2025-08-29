#include "LogsManager.h"

#include <cstdlib>

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  static std::queue<LogEntry> logQueue;
  static std::mutex logMutex;
  static std::condition_variable logCv;
  static std::atomic<int> instanceCount{0};
  static std::atomic<bool> loggingActive{false};
  static std::thread logThread;
  static std::mutex startStopMutex;

  static void logWorker() {
    while (loggingActive || !logQueue.empty()) {
      std::unique_lock<std::mutex> lock(logMutex);
      logCv.wait(lock, [] { return !logQueue.empty() || !loggingActive; });

      while (!logQueue.empty()) {
        LogEntry entry = logQueue.front();
        logQueue.pop();
        lock.unlock();

        if (entry.stream) {
          (*entry.stream) << entry.message;
          entry.stream->flush();
        }

        if (entry.file && entry.file->is_open()) {
          (*entry.file) << entry.message;
          entry.file->flush();
        }

        lock.lock();
      }
    }
  }

  LogsManager::LogsManager() {
    logFilePath = Constants::DEFAULT_LOG_FILE_PATH;
    luaLogFilePath = Constants::DEFAULT_LUA_LOG_FILE_PATH;
    std::filesystem::create_directories(Constants::DEFAULT_LOGS_DIRECTORY);
    logFile.open(logFilePath, std::ios_base::app);
    luaLogFile.open(luaLogFilePath, std::ios_base::app);

    if (!logFile.is_open()) {
      logFile.open(logFilePath, std::ios_base::out);
      if (!logFile.is_open()) {
        std::cerr << "Failed to create log file: " << logFilePath << std::endl;
      }
    }

    if (!luaLogFile.is_open()) {
      luaLogFile.open(luaLogFilePath, std::ios_base::out);
      if (!luaLogFile.is_open()) {
        std::cerr << "Failed to create lua log file: " << luaLogFilePath << std::endl;
      }
    }

    bool startThread = false;
    {
      std::lock_guard<std::mutex> lock(startStopMutex);
      instanceCount++;
      if (instanceCount == 1) {
        loggingActive = true;
        startThread = true;
      }
    }

    if (startThread) {
      logThread = std::thread(logWorker);
    }
  }

  LogsManager::~LogsManager() {
    loggingActive = false;
    logCv.notify_all();
    bool stopThread = false;
    {
      std::lock_guard<std::mutex> lock(startStopMutex);
      if (--instanceCount == 0) {
        loggingActive = false;
        stopThread = true;
        logCv.notify_all();
      }
    }

    if (stopThread && logThread.joinable()) {
      logThread.join();
    }

    if (logFile.is_open()) {
      logFile.close();
    }

    if (luaLogFile.is_open()) {
      luaLogFile.close();
    }
  }

  void LogsManager::enqueueLog(const std::string& message, std::ostream* stream, std::ofstream* file) {
    {
      std::lock_guard<std::mutex> lock(logMutex);
      if (logQueue.size() >= Constants::LOG_QUEUE_MAX_SIZE) {
        logQueue.pop();
      }
      logQueue.push(LogEntry{message, stream, file});
    }
    logCv.notify_one();
  }

  bool LogsManager::checkAndLogError(bool condition, const std::string& errorMsg) {
    if (condition) {
      logError(errorMsg);
    }
    return condition;
  }

  void LogsManager::logError(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::string sanitizedMessage = sanitizePath(message);
    std::string logMessage = Constants::LOG_ERROR + timestamp + " - " + sanitizedMessage + "\n";
    enqueueLog(logMessage, &std::cout, &logFile);
    showErrorPopup(sanitizedMessage);
  }

  void LogsManager::logLuaMessage(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::string sanitizedMessage = sanitizePath(message);
    std::string logMessage = Constants::LOG_LUA + timestamp + " - " + sanitizedMessage + "\n";
    enqueueLog(logMessage, &std::cout, &luaLogFile);
  }

  void LogsManager::logMessage(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::string sanitizedMessage = sanitizePath(message);
    std::string logMessage = Constants::LOG_INFO + timestamp + " - " + sanitizedMessage + "\n";
    enqueueLog(logMessage, &std::cout, &logFile);
  }

  void LogsManager::logWarning(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::string sanitizedMessage = sanitizePath(message);
    std::string logMessage = Constants::LOG_WARNING + timestamp + " - " + sanitizedMessage + "\n";
    enqueueLog(logMessage, &std::cout, &logFile);
    showWarningPopup(sanitizedMessage);
  }

  const std::string& LogsManager::getLogFilePath() const {
    return logFilePath;
  }

  const std::string& LogsManager::getLuaLogFilePath() const {
    return luaLogFilePath;
  }

  void LogsManager::showErrorPopup(const std::string& message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, Constants::ERROR_POPUP_TITLE, message.c_str(), nullptr);
  }

  void LogsManager::showWarningPopup(const std::string& message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, Constants::WARNING_POPUP_TITLE, message.c_str(), nullptr);
  }

  void LogsManager::printConsoleOnly(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::string sanitizedMessage = sanitizePath(message);
    std::string logMessage = Constants::LOG_CONSOLE + timestamp + " - " + sanitizedMessage + "\n";
    std::cout << logMessage << std::endl;
  }

  void LogsManager::flushLogs() {
    std::unique_lock<std::mutex> lock(logMutex);
    while (!logQueue.empty()) {
      lock.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(Constants::LOG_FLUSH_WAIT_MS));
      lock.lock();
    }

    if (logFile.is_open()) {
      logFile.flush();
      logFile.clear();
    }

    if (luaLogFile.is_open()) {
      luaLogFile.flush();
      luaLogFile.clear();
    }
  }


  std::string LogsManager::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream timestampStream;
    timestampStream << (localTime->tm_year + Constants::TIME_YEAR_OFFSET) << "-"
                    << (localTime->tm_mon + Constants::TIME_MONTH_OFFSET) << "-"
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
