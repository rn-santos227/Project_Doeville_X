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

        SDL_Color color = getLuaSDLColor(lua);
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);

      }
    }
  }

  Uint8 ComponentFactory::getLuaColorChannel(lua_State* lua, const std::string& globalName, Uint8 defaultValue = 255) {
    lua_getglobal(lua, globalName.c_str());
    Uint8 value = defaultValue;
    if (lua_isinteger(lua, -1)) {
      value = static_cast<Uint8>(lua_tointeger(lua, -1));
    }
    lua_pop(lua, 1);
    return value;
  }

  SDL_Color ComponentFactory::getLuaSDLColor(lua_State* lua) {
    SDL_Color color;
    color.r = getLuaColorChannel(lua, "fontColorR");
    color.g = getLuaColorChannel(lua, "fontColorG");
    color.b = getLuaColorChannel(lua, "fontColorB");
    color.a = getLuaColorChannel(lua, "fontColorA");
    return color;
  }
}