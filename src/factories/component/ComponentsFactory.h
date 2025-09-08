#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "assets/AssetsManager.h"
#include "components/BaseComponent.h"
#include "components/ComponentTypeResolver.h"
#include "components/camera_component/CameraComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/button_component/ButtonComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "components/input_component/InputComponent.h"
#include "components/light_component/LightComponent.h"
#include "components/modal_component/ModalComponent.h"
#include "components/meter_component/MeterComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/numeric_component/NumericComponent.h"
#include "components/network_component/NetworkComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/portal_component/PortalComponent.h"
#include "components/spawner_component/SpawnerComponent.h"
#include "components/text_component/TextComponent.h"
#include "components/timer_component/TimerComponent.h"
#include "components/transform_component/TransformComponent.h"
#include "components/vision_component/VisionComponent.h"
#include "libraries/constants/Constants.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "helpers/component_pool/ComponentPool.h"
#include "helpers/component_pool/MotionComponentPool.h"
#include "helpers/component_pool/TransformComponentPool.h"
#include "services/network/NetworkService.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project::Factories {
  class ComponentsFactory {
  public:
    using ComponentPtr = std::unique_ptr<Project::Components::BaseComponent, std::function<void(Project::Components::BaseComponent*)>>;
    ComponentsFactory(
      Project::Utilities::LogsManager& logsManager,
      Project::Utilities::ConfigReader& configReader,
      Project::Handlers::ResourcesHandler& resourcesHandler
    );

    void configurePools();
    
    ComponentPtr create(const std::string& componentName, Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName);

    void setRenderer(SDL_Renderer* renderer);
    void setCameraHandler(Project::Handlers::CameraHandler* _handler);
    void setCursorHandler(Project::Handlers::CursorHandler* _handler);
    void setKeyHandler(Project::Handlers::KeyHandler* _handler);
    void setMouseHandler(Project::Handlers::MouseHandler* _handler);
    void setAssetsManager(Project::Assets::AssetsManager* _manager);
    void setNetworkService(Project::Services::NetworkService* _service);

  private:
    SDL_Renderer* renderer = nullptr;

    Project::Assets::AssetsManager* assetsManager = nullptr;
    Project::Handlers::CameraHandler* cameraHandler = nullptr;
    Project::Handlers::CursorHandler* cursorHandler = nullptr;
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    Project::Handlers::MouseHandler* mouseHandler = nullptr;
    Project::Services::NetworkService* networkService = nullptr;
    
    Project::Utilities::ConfigReader& configReader;
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;
  };
}

#endif
