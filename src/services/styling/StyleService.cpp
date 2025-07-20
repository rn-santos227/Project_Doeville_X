#include "StyleService.h"

#include <fstream>
#include <iterator>
#include <sstream>

#include "libraries/constants/Constants.h"

namespace Project::Services {
  namespace Constants = Project::Libraries::Constants;
  namespace fs = std::filesystem;

  StyleService::StyleService(Project::Utilities::LogsManager& logsManager, Project::Handlers::ResourcesHandler& resourcesHandler)
    : logsManager(logsManager), resourcesHandler(resourcesHandler) {}

  void StyleService::loadStylesFromFolder(const std::string& folderPath) {
    std::string absPath = folderPath;
    if (!fs::path(absPath).is_absolute()) {
      absPath = resourcesHandler.getResourcePath(folderPath);
    }

    try {
      for (const auto& entry : fs::recursive_directory_iterator(absPath, fs::directory_options::skip_permission_denied)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != Constants::SCRIPT_CSS_SUFFIX) continue;
        loadStyleFile(entry.path().string());
      }
    } catch (const fs::filesystem_error& e) {
      logsManager.logError(std::string("Error iterating styles directory: ") + e.what());
    }
  }

  void StyleService::loadStyleFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
      logsManager.logError("Failed to open css file: " + filePath);
      return;
    }
    
    std::string css((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (css.empty()) {
      logsManager.logWarning("CSS file is empty: " + filePath);
      return;
    }

    try {
      StyleManager::getInstance().loadFromString(css);
      logsManager.logMessage("Loaded style sheet: " + filePath);
    } catch (const std::exception& e) {
      logsManager.logError("Failed to parse style file: " + filePath + " - " + e.what());
    }
  }
}
