#ifndef LAYERS_FACTORY_H
#define LAYERS_FACTORY_H

#include <memory>
#include <string>

#include "layers/Layer.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Factories {
  class LayersFactory {
  public:
    explicit LayersFactory(Project::Utilities::LogsManager& logsManager);
    std::unique_ptr<Project::Layers::Layer> createLayerFromLua(const std::string& scriptPath);

  private:
    Project::Utilities::LogsManager& logsManager;
  };
}

#endif
