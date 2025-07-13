#include "Layer.h"

#include "libraries/constants/Constants.h"

namespace Project::Layers {
  namespace Constants = Project::Libraries::Constants;

  Layer::Layer(const std::string& name, LayerCategory category)
    : Layer(name, category, std::make_shared<Project::Entities::EntitiesManager>()) {}

  Layer::Layer(const std::string& layerName,  LayerCategory category, std::shared_ptr<Project::Entities::EntitiesManager> entitiesManager)
    : name(layerName), category(category), visible(true), active(true), entitiesManager(std::move(entitiesManager)) {
    switch (category) {
      case LayerCategory::HUD:
        followCamera = false;
        interactable = true;
        break;
      
      case LayerCategory::OVERLAY:
        followCamera = false;
        interactable = false;
        break;
      
      default:
        followCamera = true;
        interactable = true;
        break;
    }
  }

  void Layer::update(float deltaTime) {
    if (active && entitiesManager) {
      entitiesManager->update(deltaTime);
    }
  }

  void Layer::render() {
    if (visible && entitiesManager) {
      entitiesManager->render();
    }
  }
}
