#include "ComponentsFactory.h"

namespace Project::Factories {
  ComponentsFactory::ComponentsFactory(ConfigReader& configReader, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : configReader(configReader), logsManager(logsManager), resourcesHandler(resourcesHandler), renderer(nullptr) {}

  std::unique_ptr<BaseComponent> ComponentsFactory::create(const std::string& componentName, LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    if(logsManager.checkAndLogError(!renderer, "Renderer is null for component: " + componentName)) {
      return nullptr;
    }
    
    ComponentType type = ComponentTypeResolver::resolve(componentName);
    
    switch (type) {
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
  std::unique_ptr<BaseComponent> ComponentsFactory::createGraphicsComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string imagePath = luaStateWrapper.getTableString(tableName, "imagePath", "assets/images/default.png");

    auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);
    graphicsComponent->setTexture(resourcesHandler, imagePath);
    graphicsComponent->onAttach();

    return graphicsComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createTextComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    SDL_Color color = {
      static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "fontColorR", 255)),
      static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, "fontColorG", 255)),
    };
    std::string text = getLuaGlobalString(luaStateWrapper, "text", "Default Text");

    std::string defaultFontPath = configReader.getValue("Font", "default_path", "resources/fonts/system.ttf");
    std::string fontPath = getLuaGlobalString(luaStateWrapper, "fontPath", defaultFontPath);

    int defaultFontSize = configReader.getIntValue("Font", "default_size", 16);
    int fontSize = getLuaGlobalInt(luaStateWrapper, "fontSize", defaultFontSize);

    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(font == nullptr, "Failed to load font: " + fontPath)) {
      return nullptr;
    }

    return std::make_unique<TextComponent>(renderer, font, color, text, fontPath, fontSize, logsManager);
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
}
