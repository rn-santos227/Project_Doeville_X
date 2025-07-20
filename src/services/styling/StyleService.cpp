#include "StyleService.h"

#include <fstream>
#include <sstream>

#include "libraries/constants/Constants.h"

namespace Project::Services {
  namespace Constants = Project::Libraries::Constants;
  namespace fs = std::filesystem;

  StyleService::StyleService(Project::Utilities::LogsManager& logsManager, Project::Handlers::ResourcesHandler& resourcesHandler)
    : logsManager(logsManager), resourcesHandler(resourcesHandler) {}

  void StyleService::loadStylesFromFolder(const std::string& folderPath) {
    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
      if (!entry.is_regular_file()) continue;
      const std::string path = entry.path().string();
      if (path.find(Constants::SCRIPT_CSS_SUFFIX) == std::string::npos) continue;
      loadStyleFile(path);
    }
  }

  void StyleService::loadStyleFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
      logsManager.logError("Failed to open css file: " + filePath);
      return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    StyleManager::getInstance().loadFromString(buffer.str());
    logsManager.logMessage("Loaded style sheet: " + filePath);
  }
}
