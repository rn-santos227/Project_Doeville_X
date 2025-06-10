#include "ComponentsFactory.h"

namespace Project::Factories {
  ComponentsFactory::ComponentsFactory(ConfigReader& configReader, LogsManager& , ResourcesHandler& resourcesHandler)
  : configReader(configReader), logsManager(logsManager), resourcesHandler(resourcesHandler), renderer(nullptr) {}

  std::unique_ptr<BaseComponent> ComponentsFactory::create(const std::string& componentName, LuaStateWrapper& luaStateWrapper) {
    if(logsManager.checkAndLogError(!renderer, "Renderer is null for component: " + componentName)) {
      return nullptr;
    }
    
    ComponentType type = ComponentTypeResolver::resolve(componentName);
    
    switch (type) {
      case ComponentType::GRAPHICS:
        return createGraphicsComponent(luaStateWrapper);

      case ComponentType::TEXT:
        return createTextComponent(luaStateWrapper);

      default:
        logsManager.logError("Unknown or unsupported component type: " + componentName);
        return nullptr;
    }
  }

  // Components Builder Section
  std::unique_ptr<BaseComponent> ComponentsFactory::createGraphicsComponent(LuaStateWrapper& luaStateWrapper) {
    std::string imagePath = getLuaGlobalString(luaStateWrapper, "imagePath", "assets/images/default.png");

    auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);
    graphicsComponent->setTexture(imagePath);
    graphicsComponent->onAttach();

    return graphicsComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createTextComponent(lua_State* lua) {
    SDL_Color color = getLuaSDLColor(lua);
    std::string text = getLuaGlobalString(lua, "text", "Default Text");

    std::string defaultFontPath = configReader.getValue("Paths", "default_font", "resources/fonts/system.ttf");
    std::string fontPath = getLuaGlobalString(lua, "fontPath", defaultFontPath);

    int defaultFontSize = configReader.getIntValue("Text", "default_font_size", 16);
    int fontSize = getLuaGlobalInt(lua, "fontSize", defaultFontSize);

    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(font == nullptr, "Failed to load font: " + fontPath)) {
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

  // Getters and Setters Section
  void ComponentsFactory::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
  }
}
