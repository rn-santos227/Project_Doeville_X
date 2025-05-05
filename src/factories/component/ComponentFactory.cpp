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
        lua_pop(lua, 1);

        lua_getglobal(lua, "fontSize");
        int fontSize = lua_isinteger(lua, -1) ? lua_tointeger(lua, -1) : 16;
        lua_pop(lua, 1);

        SDL_Color color = {255, 255, 255, 255};
        lua_getglobal(lua, "fontColorR"); 
        if (lua_isinteger(lua, -1)) color.r = (Uint8)lua_tointeger(lua, -1); 
        lua_pop(lua, 1);
        
        lua_getglobal(lua, "fontColorG"); 
        if (lua_isinteger(lua, -1)) color.g = (Uint8)lua_tointeger(lua, -1); 
        lua_pop(lua, 1);

        lua_getglobal(lua, "fontColorB");
        if (lua_isinteger(lua, -1)) color.b = (Uint8)lua_tointeger(lua, -1);
        lua_pop(lua, 1);

        lua_getglobal(lua, "fontColorA");
      }
    }
  }
}