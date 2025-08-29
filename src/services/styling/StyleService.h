#ifndef STYLE_SERVICE_H
#define STYLE_SERVICE_H

#include "StyleManager.h"

#include <filesystem>
#include <string>

#include "handlers/resources/ResourcesHandler.h"
#include "utilities/binary_cache/BinaryFileCache.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Services {
  class StyleService {
  public:
    StyleService(Project::Utilities::LogsManager& logsManager, Project::Handlers::ResourcesHandler& resourcesHandler);
    ~StyleService();
    
    void loadStylesFromFolder(const std::string& folderPath);

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::BinaryFileCache styleCache;
    Project::Handlers::ResourcesHandler& resourcesHandler;

    void loadStyleFile(const std::string& filePath);
  };
}

#endif
