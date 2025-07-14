#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "ComponentTypeResolver.h"

#include "components/BaseComponent.h"
#include "components/camera_component/CameraComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/text_component/TextComponent.h"
#include "components/transform_component/TransformComponent.h"
#include "libraries/constants/Constants.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Constants = Project::Libraries::Constants;
namespace Project::Factories {
  class ComponentsFactory {
  public:
    ComponentsFactory(
      Project::Utilities::ConfigReader& configReader,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler);

    std::unique_ptr<Project::Components::BaseComponent> create(const std::string& componentName, Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName);

    void setRenderer(SDL_Renderer* renderer);
    void setCameraHandler(Project::Handlers::CameraHandler* handler);
    void setKeyHandler(Project::Handlers::KeyHandler* keyHandler);

  private:
    SDL_Renderer* renderer = nullptr;
    
    Project::Handlers::CameraHandler* cameraHandler = nullptr;
    Project::Handlers::KeyHandler* keyHandler = nullptr;

    Project::Utilities::ConfigReader& configReader;
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;
  };
}

#endif
