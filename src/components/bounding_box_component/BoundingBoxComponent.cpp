#include "BoundingBoxComponent.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::GeometryUtils;
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

  void BoundingBoxComponent::addCircle(int x, int y, int r) {
    circles.push_back({x, y, r});
    updateWorldBoxes();
  }

  void BoundingBoxComponent::clearShapes() {
    boxes.clear();
    worldBoxes.clear();
    circles.clear();
    worldCircles.clear();
  }

  const std::vector<SDL_Rect>& BoundingBoxComponent::getBoxes() const {
    return worldBoxes;
  }

  const std::vector<Project::Utilities::Circle>& BoundingBoxComponent::getCircles() const {
    return worldCircles;
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
    worldCircles.resize(circles.size());

    for (size_t i = 0; i < boxes.size(); ++i) {
      worldBoxes[i].x = boxes[i].x + entityX;
      worldBoxes[i].y = boxes[i].y + entityY;
      worldBoxes[i].w = boxes[i].w;
      worldBoxes[i].h = boxes[i].h;
    }

    for (size_t i = 0; i < circles.size(); ++i) {
      worldCircles[i].x = circles[i].x + entityX;
      worldCircles[i].y = circles[i].y + entityY;
      worldCircles[i].r = circles[i].r;
    }
  }
}
