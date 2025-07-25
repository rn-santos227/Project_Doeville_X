#include "ComponentsFactory.h"

#include "handlers/input/KeyActionResolver.h"
#include "handlers/input/KeyCodeResolver.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Factories {
  using Project::Components::BaseComponent;
  using Project::Components::BoundingBoxComponent;
  using Project::Components::ButtonComponent;
  using Project::Components::CameraComponent;
  using Project::Components::GraphicsComponent;
  using Project::Components::KeysComponent;
  using Project::Components::MotionComponent;
  using Project::Components::NumericComponent;
  using Project::Components::PhysicsComponent;
  using Project::Components::SpawnerComponent;
  using Project::Components::TextComponent;
  using Project::Components::TimerComponent;
  using Project::Components::TransformComponent;
  using Project::Handlers::CameraHandler;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::KeyHandler;
  using Project::Handlers::MouseHandler;
  using Project::Handlers::ResourcesHandler;
  using Project::Utilities::ColorUtils;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  ComponentsFactory::ComponentsFactory(ConfigReader& configReader, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : configReader(configReader), logsManager(logsManager),
    resourcesHandler(resourcesHandler),
    renderer(nullptr), keyHandler(nullptr),
    mouseHandler(nullptr), cursorHandler(nullptr) {}

  ComponentsFactory::ComponentPtr ComponentsFactory::create(const std::string& componentName, LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    if (logsManager.checkAndLogError(!renderer, "Renderer is null for component: " + componentName)) {
      return nullptr;
    }

    std::string resolvedName = luaStateWrapper.getTableString(tableName, Keys::COMPONENT, componentName);
    ComponentType type = ComponentTypeResolver::resolve(resolvedName);

    switch (type) {
      case ComponentType::BOUNDING_BOX: {
        SDL_Color defaultColor = Constants::DEFAULT_DEBUG_TEXT_COLOR;
        SDL_Color debugColor = configReader.getColorValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_COLOR, defaultColor);
        auto component = Project::Helpers::ComponentPool<BoundingBoxComponent>::getInstance().acquire(logsManager, renderer, keyHandler, debugColor);
        component->build(luaStateWrapper, tableName);   
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<BoundingBoxComponent*>(b)); });
        return base;
      }

      case ComponentType::BUTTON: {
        auto component = Project::Helpers::ComponentPool<ButtonComponent>::getInstance().acquire(renderer, logsManager, configReader, mouseHandler, cursorHandler);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<ButtonComponent*>(b)); });
        return base;
      }

      case ComponentType::CAMERA: {
        auto component = Project::Helpers::ComponentPool<CameraComponent>::getInstance().acquire(logsManager, cameraHandler);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<CameraComponent*>(b)); });
        return base;
      }

      case ComponentType::GRAPHICS: {
        auto component = Project::Helpers::ComponentPool<GraphicsComponent>::getInstance().acquire(renderer, &resourcesHandler, logsManager);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<GraphicsComponent*>(b)); });
        return base;
      }

      case ComponentType::KEYS: {
        auto component = Project::Helpers::ComponentPool<KeysComponent>::getInstance().acquire(logsManager, keyHandler);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<KeysComponent*>(b)); });
        return base;
      }

      case ComponentType::MOTION: {
        auto component = Project::Helpers::ComponentPool<MotionComponent>::getInstance().acquire(logsManager, keyHandler);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<MotionComponent*>(b)); });
        return base;
      }

      case ComponentType::NUMERIC: {
        auto component = Project::Helpers::ComponentPool<NumericComponent>::getInstance().acquire(logsManager);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<NumericComponent*>(b)); });
        return base;
      }

      case ComponentType::PHYSICS: {
        auto component = Project::Helpers::ComponentPool<PhysicsComponent>::getInstance().acquire(logsManager);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<PhysicsComponent*>(b)); });
        return base;
      }

      case ComponentType::SPAWNER: {
        auto component = Project::Helpers::ComponentPool<SpawnerComponent>::getInstance().acquire(logsManager);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<SpawnerComponent*>(b)); });
        return base;
      }

      case ComponentType::TEXT: {
        auto component = Project::Helpers::ComponentPool<TextComponent>::getInstance().acquire(renderer, configReader, logsManager);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<TextComponent*>(b)); });
        return base;
      }

      case ComponentType::TIMER: {
        auto component = Project::Helpers::ComponentPool<TimerComponent>::getInstance().acquire(logsManager);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));

        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<TimerComponent*>(b)); });
        return base;
      }

      case ComponentType::TRANSFORM: {
        auto component = Project::Helpers::ComponentPool<TransformComponent>::getInstance().acquire(logsManager);
        component->build(luaStateWrapper, tableName);
        component->setActive(luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true));
        component->setClass(luaStateWrapper.getTableString(tableName, Keys::CLASS, Constants::EMPTY_STRING));
        
        ComponentPtr base(component.release(), [d = component.get_deleter()](BaseComponent* b){ d(static_cast<TransformComponent*>(b)); });
        return base;
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

  void ComponentsFactory::setCameraHandler(CameraHandler* _handler) {
    this->cameraHandler = _handler;
  }

  void ComponentsFactory::setCursorHandler(CursorHandler* _handler) {
    this->cursorHandler = _handler;
  }

  void ComponentsFactory::setKeyHandler(KeyHandler* _handler) {
    this->keyHandler = _handler;
  }

  void ComponentsFactory::setMouseHandler(MouseHandler* _handler) {
    this->mouseHandler = _handler;
  }
}
