#include "ComponentsFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
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

    std::string resolvedName = luaStateWrapper.getTableString(tableName, Project::Libraries::Keys::COMPONENT, componentName);
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
    SDL_Color debugColor = configReader.getColorValue(Project::Libraries::Keys::FONT_SECTION, Project::Libraries::Keys::FONT_DEFAULT_COLOR, defaultColor);
    auto boxComponent = std::make_unique<BoundingBoxComponent>(logsManager, renderer, keyHandler, debugColor);

    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, Project::Libraries::Keys::BOXES);
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_istable(L, -1)) {
            int x = 0, y = 0, w = 0, h = 0;
            lua_getfield(L, -1, Project::Libraries::Keys::X);
            if (lua_isnumber(L, -1)) x = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Project::Libraries::Keys::Y);
            if (lua_isnumber(L, -1)) y = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Project::Libraries::Keys::W);
            if (lua_isnumber(L, -1)) w = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Project::Libraries::Keys::H);
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

    bool active = luaStateWrapper.getTableBoolean(tableName, Project::Libraries::Keys::ACTIVE, true);
    boxComponent->setActive(active);

    bool solid = luaStateWrapper.getTableBoolean(tableName, Project::Libraries::Keys::SOLID, false);
    boxComponent->setSolid(solid);

    return boxComponent;
 }

  std::unique_ptr<BaseComponent> ComponentsFactory::createGraphicsComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string imagePath = luaStateWrapper.getTableString(tableName, Project::Libraries::Keys::TEXTURE_PATH, "");

    auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);

    if (!imagePath.empty()) {
      graphicsComponent->setTexture(resourcesHandler, imagePath);
    } else {
      int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Project::Libraries::Keys::WIDTH, Project::Libraries::Constants::DEFAULT_COMPONENT_SIZE));
      int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Project::Libraries::Keys::HEIGHT, Project::Libraries::Constants::DEFAULT_COMPONENT_SIZE));
      std::string colorHex = luaStateWrapper.getTableString(tableName, Project::Libraries::Keys::COLOR_HEX, Project::Libraries::Constants::DEFAULT_SHAPE_COLOR_HEX);
      Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Project::Libraries::Keys::COLOR_ALPHA, Project::Libraries::Constants::FULL_ALPHA));
      SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);
      graphicsComponent->setShape(width, height, color);
    }

    bool active = luaStateWrapper.getTableBoolean(tableName, Project::Libraries::Keys::ACTIVE, true);
    graphicsComponent->setActive(active);

    graphicsComponent->onAttach();
    return graphicsComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createTextComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string colorHex = luaStateWrapper.getTableString(tableName, Project::Libraries::Keys::FONT_COLOR_HEX, Project::Libraries::Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Project::Libraries::Keys::FONT_COLOR_ALPHA, Project::Libraries::Constants::FULL_ALPHA));
    SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);

    std::string text = luaStateWrapper.getTableString(tableName, Project::Libraries::Keys::TEXT, Project::Libraries::Keys::DEFAULT_TEXT);

    std::string defaultFontPath = configReader.getValue(Project::Libraries::Keys::FONT_SECTION, Project::Libraries::Keys::FONT_DEFAULT_PATH, Project::Libraries::Constants::DEFAULT_FONT_PATH);
    std::string fontPath = luaStateWrapper.getTableString(tableName, Project::Libraries::Keys::FONT_PATH, defaultFontPath);

    int defaultFontSize = configReader.getIntValue(Project::Libraries::Keys::FONT_SECTION, Project::Libraries::Keys::FONT_DEFAULT_SIZE, Project::Libraries::Constants::DEFAULT_FONT_SIZE);
    int fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Project::Libraries::Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));

    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(font == nullptr, "Failed to load font: " + fontPath)) {
      return nullptr;
    }

    auto textComponent = std::make_unique<TextComponent>(renderer, font, color, text, fontPath, fontSize, logsManager);
    bool active = luaStateWrapper.getTableBoolean(tableName, Project::Libraries::Keys::ACTIVE, true);
    textComponent->setActive(active);
    return textComponent;
  }

  // Utilities Section
  Uint8 ComponentsFactory::getLuaColorChannel(LuaStateWrapper& luaStateWrapper, const std::string& globalName, Uint8 defaultValue) {
    return static_cast<Uint8>(luaStateWrapper.getGlobalNumber(globalName, static_cast<float>(defaultValue)));
  }

  SDL_Color ComponentsFactory::getLuaSDLColor(LuaStateWrapper& luaStateWrapper) {
    std::string hex = luaStateWrapper.getGlobalString(Project::Libraries::Keys::FONT_COLOR_HEX, Project::Libraries::Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = getLuaColorChannel(luaStateWrapper, Project::Libraries::Keys::FONT_COLOR_ALPHA);
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
