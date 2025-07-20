#include "ComponentsFactory.h"

#include "handlers/input/KeyActionResolver.h"
#include "handlers/input/KeyCodeResolver.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Factories {
  using Project::Components::CameraComponent;
  using Project::Components::BaseComponent;
  using Project::Components::BoundingBoxComponent;
  using Project::Components::GraphicsComponent;
  using Project::Components::KeysComponent;
  using Project::Components::MotionComponent;
  using Project::Components::PhysicsComponent;
  using Project::Components::TextComponent;
  using Project::Components::TimerComponent;
  using Project::Components::TransformComponent;
  using Project::Handlers::ResourcesHandler;
  using Project::Utilities::ColorUtils;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  ComponentsFactory::ComponentsFactory(ConfigReader& configReader, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : configReader(configReader), logsManager(logsManager), resourcesHandler(resourcesHandler), renderer(nullptr), keyHandler(nullptr) {}

  std::unique_ptr<BaseComponent> ComponentsFactory::create(const std::string& componentName, LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    if (logsManager.checkAndLogError(!renderer, "Renderer is null for component: " + componentName)) {
      return nullptr;
    }

    std::string resolvedName = luaStateWrapper.getTableString(tableName, Keys::COMPONENT, componentName);
    ComponentType type = ComponentTypeResolver::resolve(resolvedName);

    switch (type) {
      case ComponentType::BOUNDING_BOX: {
        SDL_Color defaultColor = Constants::DEFAULT_DEBUG_TEXT_COLOR;
        SDL_Color debugColor = configReader.getColorValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_COLOR, defaultColor);
        auto component = std::make_unique<BoundingBoxComponent>(logsManager, renderer, keyHandler, debugColor);
        
        component->build(luaStateWrapper, tableName);    
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      case ComponentType::CAMERA: {
        auto component = std::make_unique<CameraComponent>(logsManager, cameraHandler);
        
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      case ComponentType::GRAPHICS: {
        auto component = std::make_unique<GraphicsComponent>(renderer, &resourcesHandler, logsManager);
        
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      case ComponentType::KEYS: {
        auto component = std::make_unique<KeysComponent>(logsManager, keyHandler);
        
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      case ComponentType::MOTION: {
        auto component = std::make_unique<MotionComponent>(logsManager, keyHandler);
        
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      case ComponentType::PHYSICS: {
        auto component = std::make_unique<PhysicsComponent>(logsManager);
        
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      case ComponentType::TEXT: {
        auto component = std::make_unique<TextComponent>(renderer, configReader, logsManager);
        
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      case ComponentType::TIMER: {
        auto component = std::make_unique<TimerComponent>(logsManager);

        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        return component;
      }

      case ComponentType::TRANSFORM: {
        auto component = std::make_unique<TransformComponent>(logsManager);
        
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        return component;
      }

      default: {
        logsManager.logError("Unknown or unsupported component type: " + componentName);
        return nullptr;
      }
    }
  }

  // Getters and Setters Section
  void ComponentsFactory::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
  }

  void ComponentsFactory::setCameraHandler(Handlers::CameraHandler* handler) {
    this->cameraHandler = handler;
  }

  void ComponentsFactory::setKeyHandler(Handlers::KeyHandler* keyHandler) {
    this->keyHandler = keyHandler;
  }
}
