#include "ComponentFactory.h"

namespace Project::Factories {
  ComponentFactory::ComponentFactory(SDL_Renderer* renderer, LogsManager& logsManager)
  : renderer(renderer), logsManager(logsManager) {}

  std::unique_ptr<BaseComponent> ComponentFactory::create(const std::string& componentName, lua_State* lua) {
    ComponentType type = ComponentTypeResolver::resolve(componentName);
    
    switch (type) {
      case ComponentType::GRAPHICS: {
        
      }
    }
  }
}