#include "ComponentFactory.h"

namespace Project::Factories {
  ComponentFactory::ComponentFactory(SDL_Renderer* renderer, LogsManager& logsManager)
  : renderer(renderer), logsManager(logsManager) {}

  std::unique_ptr<BaseComponent> ComponentFactory::create(const std::string& componentName, lua_State* lua) {
    ComponentType type = ComponentTypeResolver::resolve(componentName);
    
    switch (type) {
      case ComponentType::GRAPHICS: {
        auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);
        graphicsComponent->onAttach();
        return graphicsComponent;
      }

      case ComponentType::TEXT: {
        lua_getglobal(lua, "text");
        std::string text = lua_isstring(lua, -1) ? lua_tostring(lua, -1) : "Default Text";
        lua_pop(lua, 1);

        lua_getglobal(lua, "fontPath");
        std::string fontPath = lua_isstring(lua, -1) ? lua_tostring(lua, -1) : "assets/fonts/default.ttf";
      }
    }
  }
}