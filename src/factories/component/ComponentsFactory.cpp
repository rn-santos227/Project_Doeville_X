#include "ComponentsFactory.h"

namespace Project::Factories {
  ComponentsFactory::ComponentsFactory(ConfigReader& configReader, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : configReader(configReader), logsManager(logsManager), resourcesHandler(resourcesHandler), renderer(nullptr), keyHandler(nullptr) {}

  std::unique_ptr<BaseComponent> ComponentsFactory::create(const std::string& componentName, LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    if(logsManager.checkAndLogError(!renderer, "Renderer is null for component: " + componentName)) {
      return nullptr;
    }
    
    std::string resolvedName = luaStateWrapper.getTableString(tableName, "component", componentName);
    ComponentType type = ComponentTypeResolver::resolve(resolvedName);
    
    switch (type) {
      case ComponentType::BOUNDING_BOX:
        return createBoundingBoxComponent(luaStateWrapper, tableName);

      case ComponentType::GRAPHICS:
        return createGraphicsComponent(luaStateWrapper, tableName);

      case ComponentType::TEXT:
        return createTextComponent(luaStateWrapper, tableName);

      default:
        logsManager.logError("Unknown or unsupported component type: " + componentName);
        return nullptr;
    }
  }

  // Components Builder Section
 std::unique_ptr<BaseComponent> ComponentsFactory::createBoundingBoxComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    SDL_Color defaultColor{144, 238, 144, 255};
    SDL_Color debugColor = configReader.getColorValue("Font", "default_color", defaultColor);
    auto boxComponent = std::make_unique<BoundingBoxComponent>(renderer, keyHandler, debugColor, logsManager);

    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, "boxes");
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_istable(L, -1)) {
            int x = 0, y = 0, w = 0, h = 0;
            lua_getfield(L, -1, "x");
            if (lua_isnumber(L, -1)) x = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, "y");
            if (lua_isnumber(L, -1)) y = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, "w");
            if (lua_isnumber(L, -1)) w = static_cast<int>(lua_tonumber(L, -1));
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }
 }

  std::unique_ptr<BaseComponent> ComponentsFactory::createGraphicsComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string imagePath = luaStateWrapper.getTableString(tableName, "imagePath", "assets/images/default.png");

    auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);
    graphicsComponent->setTexture(resourcesHandler, imagePath);

    bool active = luaStateWrapper.getTableBoolean(tableName, "active", true);
    graphicsComponent->setActive(active);

    graphicsComponent->onAttach();
    return graphicsComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createTextComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    SDL_Color color = {
      static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "fontColorR", 255)),
      static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "fontColorG", 255)),
      static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "fontColorB", 255)),
      static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "fontColorA", 255))
    };
    std::string text = luaStateWrapper.getTableString(tableName, "text", "Default Text");

    std::string defaultFontPath = configReader.getValue("Font", "default_path", "resources/fonts/system.ttf");
    std::string fontPath = luaStateWrapper.getTableString(tableName, "fontPath", defaultFontPath);

    int defaultFontSize = configReader.getIntValue("Font", "default_size", 16);
    int fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, "fontSize", static_cast<float>(defaultFontSize)));

    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(font == nullptr, "Failed to load font: " + fontPath)) {
      return nullptr;
    }

    auto textComponent = std::make_unique<TextComponent>(renderer, font, color, text, fontPath, fontSize, logsManager);
    bool active = luaStateWrapper.getTableBoolean(tableName, "active", true);
    textComponent->setActive(active);
    return textComponent;
  }

  // Utilities Section
  Uint8 ComponentsFactory::getLuaColorChannel(LuaStateWrapper& luaStateWrapper, const std::string& globalName, Uint8 defaultValue) {
    return static_cast<Uint8>(luaStateWrapper.getGlobalNumber(globalName, static_cast<float>(defaultValue)));
  }

  SDL_Color ComponentsFactory::getLuaSDLColor(LuaStateWrapper& luaStateWrapper) {
    SDL_Color color;
    color.r = getLuaColorChannel(luaStateWrapper, "fontColorR");
    color.g = getLuaColorChannel(luaStateWrapper, "fontColorG");
    color.b = getLuaColorChannel(luaStateWrapper, "fontColorB");
    color.a = getLuaColorChannel(luaStateWrapper, "fontColorA");
    return color;
  }

  std::string ComponentsFactory::getLuaGlobalString(LuaStateWrapper& lua, const std::string& name, const std::string& defaultValue) {
    return lua.getGlobalString(name, defaultValue);
  }

  int ComponentsFactory::getLuaGlobalInt(LuaStateWrapper& lua, const std::string& name, int defaultValue) {
    return static_cast<int>(lua.getGlobalNumber(name, static_cast<float>(defaultValue)));
  }


  // Getters and Setters Section
  void ComponentsFactory::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
  }

  void ComponentsFactory::setKeyHandler(Handlers::KeyHandler* keyHandler) {
    this->keyHandler = keyHandler;
  }
}
