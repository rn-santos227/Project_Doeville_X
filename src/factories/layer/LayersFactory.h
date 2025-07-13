#ifndef LAYERS_FACTORY_H
#define LAYERS_FACTORY_H

#include <memory>
#include <string>
#include <unordered_map>

#include "layers/Layer.h"
#include "layers/LayerCategoryResolver.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Factories {
  class LayersFactory {
  public:
    explicit LayersFactory(Project::Utilities::LogsManager& logsManager);
    std::unique_ptr<Project::Layers::Layer> createLayerFromLua(const std::string& scriptPath);
    std::unique_ptr<Project::Layers::Layer> cloneLayer(const std::string& layerName);
    std::unique_ptr<Project::Layers::Layer> cloneLayerFromPath(const std::string& scriptPath);
    
    bool hasLayerTemplate(const std::string& layerName) const;
    
  private:
    Project::Utilities::LogsManager& logsManager;
    std::unordered_map<std::string, std::unique_ptr<Project::Layers::Layer>> layerTemplates;
    std::unordered_map<std::string, std::string> layerScriptPaths;

    std::unique_ptr<Project::Layers::Layer> loadLayerTemplateFromLua(const std::string& scriptPath);
  };
}

#endif
