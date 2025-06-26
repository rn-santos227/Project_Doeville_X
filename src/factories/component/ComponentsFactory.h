#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "ComponentTypeResolver.h"

#include "components/BaseComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/text_component/TextComponent.h"

#include "handlers/resources/ResourcesHandler.h"

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "utilities/config_reader/ConfigReader.h"

using namespace Project::Components;
using namespace Project::Handlers;
using namespace Project::Handlers;
using namespace Project::Utilities;

namespace Project::Factories {
  class ComponentsFactory {
  public:
    ComponentsFactory(ConfigReader& configReader, LogsManager& logsManager, ResourcesHandler& resourcesHandler);

    std::unique_ptr<BaseComponent> create(const std::string& componentName, LuaStateWrapper& luaStateWrapper, const std::string& tableName);
    void setRenderer(SDL_Renderer* renderer);

  private:
    SDL_Renderer* renderer = nullptr;
    
    ConfigReader& configReader;
    LogsManager& logsManager;
    ResourcesHandler& resourcesHandler;

    //Components Builder
    std::unique_ptr<BaseComponent> createGraphicsComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName);
    std::unique_ptr<BaseComponent> createTextComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName);
    
    //Utilities
    SDL_Color getLuaSDLColor(LuaStateWrapper& luaStateWrapper);
    Uint8 getLuaColorChannel(LuaStateWrapper& luaStateWrapper, const std::string& globalName, Uint8 defaultValue = 255); 
    std::string getLuaGlobalString(LuaStateWrapper& luaStateWrapper, const std::string& name, const std::string& defaultValue);
    int getLuaGlobalInt(LuaStateWrapper& luaStateWrapper, const std::string& name, int defaultValue);
  };
}
#endif
