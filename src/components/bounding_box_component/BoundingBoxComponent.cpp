#include "BoundingBoxComponent.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Handlers::KeyHandler;
  
  BoundingBoxComponent::BoundingBoxComponent(LogsManager& logsManager, SDL_Renderer* renderer, KeyHandler* keyHandler, SDL_Color debugColor)
      : BaseComponent(logsManager), PositionableComponent(), renderer(renderer), keyHandler(keyHandler), debugColor(debugColor) {
    logsManager.logMessage("BoundingBoxComponent initialized.");
  }

  void BoundingBoxComponent::update(float /*deltaTime*/) {}

  void BoundingBoxComponent::render() {}

  void BoundingBoxComponent::addBox(const SDL_Rect& rect) {
    boxes.push_back(rect);
    updateWorldBoxes();
  }

  void BoundingBoxComponent::clearBoxes() {
    boxes.clear();
    worldBoxes.clear();
  }

  const std::vector<SDL_Rect>& BoundingBoxComponent::getBoxes() const {
    return worldBoxes;
  }

  void BoundingBoxComponent::setSolid(bool solidEnabled) {
    solid = solidEnabled;
  }

  bool BoundingBoxComponent::isSolid() const {
    return solid;
  }

  void BoundingBoxComponent::setEntityPosition(int x, int y) {
    entityX = x;
    entityY = y;
    updateWorldBoxes();
  }

  void BoundingBoxComponent::updateWorldBoxes() {
    worldBoxes.resize(boxes.size());
    for (size_t i = 0; i < boxes.size(); ++i) {
      worldBoxes[i].x = boxes[i].x + entityX;
      worldBoxes[i].y = boxes[i].y + entityY;
      worldBoxes[i].w = boxes[i].w;
      worldBoxes[i].h = boxes[i].h;
    }
  }
}
