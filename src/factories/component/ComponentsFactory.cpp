#include "ComponentsFactory.h"

namespace Project::Factories {
  ComponentsFactory::ComponentsFactory(SDL_Renderer* renderer,  ConfigHandler& configHandler, LogsManager& logsManager)
  : renderer(renderer), configHandler(configHandler), logsManager(logsManager) {}

  std::unique_ptr<BaseComponent> ComponentsFactory::create(const std::string& componentName, lua_State* lua) {
    ComponentType type = ComponentTypeResolver::resolve(componentName);
    
    switch (type) {
      case ComponentType::GRAPHICS:
        return createGraphicsComponent(lua);

      case ComponentType::TEXT:
        return createTextComponent(lua);

      default:
        logsManager.logError("Unknown or unsupported component type: " + componentName);
        return nullptr;
    }
  }

  // Components Builder Section
  std::unique_ptr<BaseComponent> ComponentsFactory::createGraphicsComponent(lua_State* lua) {
    std::string imagePath = getLuaGlobalString(lua, "imagePath", "assets/images/default.png");

    auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);
    graphicsComponent->setTexture(imagePath);
    graphicsComponent->onAttach();

    return graphicsComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createTextComponent(lua_State* lua) {
    SDL_Color color = getLuaSDLColor(lua);
    std::string text = getLuaGlobalString(lua, "text", "Default Text");

    std::string defaultFontPath = configHandler.getValue("Paths", "default_font", "resources/fonts/system.ttf");
    std::string fontPath = getLuaGlobalString(lua, "fontPath", defaultFontPath);

    int defaultFontSize = configHandler.getIntValue("Text", "default_font_size", 16);

    std::string fontPath = getLuaGlobalString(lua, "fontPath", "resources/fonts/system.ttf");
    int fontSize = getLuaGlobalInt(lua, "fontSize", 16);

    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(font == nullptr, "Failed to load font: " + fontPath)) {
      logsManager.flushLogs();
      return nullptr;
    }

    return std::make_unique<TextComponent>(renderer, font, color, text, fontPath, fontSize, logsManager);
  }

  // Utilities Section
  Uint8 ComponentsFactory::getLuaColorChannel(lua_State* lua, const std::string& globalName, Uint8 defaultValue) {
    lua_getglobal(lua, globalName.c_str());
    Uint8 value = defaultValue;
    if (lua_isinteger(lua, -1)) {
      value = static_cast<Uint8>(lua_tointeger(lua, -1));
    }
    lua_pop(lua, 1);
    return value;
  }

  SDL_Color ComponentsFactory::getLuaSDLColor(lua_State* lua) {
    SDL_Color color;
    color.r = getLuaColorChannel(lua, "fontColorR");
    color.g = getLuaColorChannel(lua, "fontColorG");
    color.b = getLuaColorChannel(lua, "fontColorB");
    color.a = getLuaColorChannel(lua, "fontColorA");
    return color;
  }

  std::string ComponentsFactory::getLuaGlobalString(lua_State* lua, const std::string& name, const std::string& defaultValue) {
    lua_getglobal(lua, name.c_str());
    std::string value = lua_isstring(lua, -1) ? lua_tostring(lua, -1) : defaultValue;
    lua_pop(lua, 1);
    return value;
  }

  int ComponentsFactory::getLuaGlobalInt(lua_State* lua, const std::string& name, int defaultValue) {
    lua_getglobal(lua, name.c_str());
    int value = lua_isinteger(lua, -1) ? lua_tointeger(lua, -1) : defaultValue;
    lua_pop(lua, 1);
    return value;
  }
}