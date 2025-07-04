#include "ComponentsFactory.h"
#include "libraries/constants/Constants.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Factories {
  using Project::Components::BaseComponent;
  using Project::Components::BoundingBoxComponent;
  using Project::Components::GraphicsComponent;
  using Project::Components::TextComponent;
  using Project::Handlers::ResourcesHandler;
  using Project::Utilities::ColorUtils;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;

  namespace Constants = Project::Libraries::Constants;

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
    SDL_Color defaultColor = Constants::DEFAULT_DEBUG_TEXT_COLOR;
    SDL_Color debugColor = configReader.getColorValue("Font", "default_color", defaultColor);
    auto boxComponent = std::make_unique<BoundingBoxComponent>(logsManager, renderer, keyHandler, debugColor);

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
            lua_pop(L, 1);
            lua_getfield(L, -1, "h");
            if (lua_isnumber(L, -1)) h = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            SDL_Rect rect{x, y, w, h};
            boxComponent->addBox(rect);
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }

    bool active = luaStateWrapper.getTableBoolean(tableName, "active", true);
    boxComponent->setActive(active);

    bool solid = luaStateWrapper.getTableBoolean(tableName, "solid", false);
    boxComponent->setSolid(solid);

    return boxComponent;
 }

  std::unique_ptr<BaseComponent> ComponentsFactory::createGraphicsComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string imagePath = luaStateWrapper.getTableString(tableName, "imagePath", "");

    auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);

    if (!imagePath.empty()) {
      graphicsComponent->setTexture(resourcesHandler, imagePath);
    } else {
      int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, "width", Constants::DEFAULT_COMPONENT_SIZE));
      int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, "height", Constants::DEFAULT_COMPONENT_SIZE));
      std::string colorHex = luaStateWrapper.getTableString(tableName, "colorHex", "FFFFFF");
      Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "colorAlpha", Constants::FULL_ALPHA));
      SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);
      graphicsComponent->setShape(width, height, color);
    }

    bool active = luaStateWrapper.getTableBoolean(tableName, "active", true);
    graphicsComponent->setActive(active);

    graphicsComponent->onAttach();
    return graphicsComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createTextComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string colorHex = luaStateWrapper.getTableString(tableName, "fontColorHex", "FFFFFF");
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "fontAlpha", 255));
    SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);

    std::string text = luaStateWrapper.getTableString(tableName, "text", "Default Text");

    std::string defaultFontPath = configReader.getValue("Font", "default_path", Constants::DEFAULT_FONT_PATH);
    std::string fontPath = luaStateWrapper.getTableString(tableName, "fontPath", defaultFontPath);

    int defaultFontSize = configReader.getIntValue("Font", "default_size", Constants::DEFAULT_FONT_SIZE);
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
    std::string hex = luaStateWrapper.getGlobalString("fontColorHex", "FFFFFF");
    Uint8 alpha = getLuaColorChannel(luaStateWrapper, "fontColorA");
    return ColorUtils::hexToRGB(hex, alpha);
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
