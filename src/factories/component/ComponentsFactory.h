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

#include "ComponentTypeResolver.h"


#include "components/BaseComponent.h"
#include "components/grahpics_component/GraphicsComponent.h"
#include "components/text_component/TextComponent.h"

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/config_reader/ConfigReader.h"

using namespace Project::Components;
using namespace Project::Handlers;
using namespace Project::Utilities;

namespace Project::Factories {
  class ComponentsFactory {
  public:
    ComponentsFactory(ConfigReader& configReader, LogsManager& logsManager);

    std::unique_ptr<BaseComponent> create(const std::string& componentName, lua_State* lua);
    void setRenderer(SDL_Renderer* renderer);

  private:
    SDL_Renderer* renderer = nullptr;
    
    ConfigReader& configReader;
    LogsManager& logsManager;

    //Components Builder
    std::unique_ptr<BaseComponent> createGraphicsComponent(lua_State* lua);
    std::unique_ptr<BaseComponent> createTextComponent(lua_State* lua);
    
    //Utilities
    SDL_Color getLuaSDLColor(lua_State* lua);
    Uint8 getLuaColorChannel(lua_State* lua, const std::string& globalName, Uint8 defaultValue = 255); 
    std::string getLuaGlobalString(lua_State* lua, const std::string& name, const std::string& defaultValue);
    int getLuaGlobalInt(lua_State* lua, const std::string& name, int defaultValue);
  };
}

#endif