#include "BoundingBoxComponent.h"

#include "libraries/keys/Keys.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::GeometryUtils;
  using Project::Handlers::KeyHandler;

  namespace Keys = Project::Libraries::Keys;
  
  BoundingBoxComponent::BoundingBoxComponent(LogsManager& logsManager, SDL_Renderer* renderer, KeyHandler* keyHandler, SDL_Color debugColor)
    : BaseComponent(logsManager), PositionableComponent(), renderer(renderer), keyHandler(keyHandler), debugColor(debugColor) {
    logsManager.logMessage("BoundingBoxComponent initialized.");
  }

  void BoundingBoxComponent::update(float /*deltaTime*/) {}

  void BoundingBoxComponent::render() {}

  void BoundingBoxComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, Keys::BOXES);
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_istable(L, -1)) {
            int x = 0, y = 0, w = 0, h = 0, r = 0;
            lua_getfield(L, -1, Keys::X);
            if (lua_isnumber(L, -1)) x = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::Y);
            if (lua_isnumber(L, -1)) y = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::W);
            if (lua_isnumber(L, -1)) w = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::H);
            if (lua_isnumber(L, -1)) h = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::RADIUS);
            if (lua_isnumber(L, -1)) r = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            if (r > 0) {
              addCircle(x, y, r);
            } else {
              SDL_Rect rect{x, y, w, h};
              addBox(rect);
            }
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }

    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    setActive(active);

    bool solidValue = luaStateWrapper.getTableBoolean(tableName, Keys::SOLID, false);
    setSolid(solidValue);

    float fric = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FRICTION, Project::Libraries::Constants::DEFAULT_FRICTION));
    setFriction(fric);

    float rest = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::RESTITUTION, Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR));
    setRestitution(rest);
  }

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
