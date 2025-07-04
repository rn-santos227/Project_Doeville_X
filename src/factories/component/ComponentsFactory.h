#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "ComponentTypeResolver.h"

#include "components/BaseComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/text_component/TextComponent.h"
#include "libraries/constants/Constants.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Constants = Project::Libraries::Constants;
namespace Project::Factories {
  class ComponentsFactory {
  public:
    ComponentsFactory(
      Project::Utilities::ConfigReader& configReader,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler);

    std::unique_ptr<Project::Components::BaseComponent> create(const std::string& componentName, Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName);

    void setKeyHandler(Project::Handlers::KeyHandler* keyHandler);
    void setRenderer(SDL_Renderer* renderer);

  private:
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    SDL_Renderer* renderer = nullptr;

    Project::Utilities::ConfigReader& configReader;
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;

    //Components Builder
    std::unique_ptr<Project::Components::BaseComponent> createBoundingBoxComponent(
      Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName);
    std::unique_ptr<Project::Components::BaseComponent> createGraphicsComponent(
      Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName);
    std::unique_ptr<Project::Components::BaseComponent> createTextComponent(
      Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName);

    //Utilities
    SDL_Color getLuaSDLColor(Project::Utilities::LuaStateWrapper& luaStateWrapper);
    Uint8 getLuaColorChannel(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& globalName, Uint8 defaultValue = Constants::FULL_ALPHA);
    std::string getLuaGlobalString(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& name, const std::string& defaultValue);
    int getLuaGlobalInt(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& name, int defaultValue);
  };
}
#endif
