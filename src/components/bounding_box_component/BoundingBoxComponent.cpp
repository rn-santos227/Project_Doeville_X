#include "BoundingBoxComponent.h"

namespace Project::Components {
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
