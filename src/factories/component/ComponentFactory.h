#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>
#include <string>

#include <SDL2/SDL.h>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "components/BaseComponent.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Components;
using namespace Project::Utilities;

namespace Project::Factories {
  class ComponentFactory {
  public:
    ComponentFactory(SDL_Renderer* renderer, LogsManager& logsManager);
    std::unique_ptr<BaseComponent> create(const std::string& componentName, lua_State* lua);

  private:
    SDL_Renderer* renderer;
    LogsManager& logsManager;
  };
}

#endif