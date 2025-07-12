#include "Layer.h"

#include "libraries/constants/Constants.h"

namespace Project::Layers {
  namespace Constants = Project::Libraries::Constants;

  Layer::Layer(const std::string& layerName,  LayerCategory category, std::shared_ptr<Project::Entities::EntitiesManager> entitiesManager)
  : name(name), category(category), visible(true), active(true), entitiesManager(std::move(entitiesManager)) {}

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
