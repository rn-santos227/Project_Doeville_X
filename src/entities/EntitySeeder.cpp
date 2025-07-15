#include "EntitySeeder.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/math/MathUtils.h"

namespace Project::Entities {
  using Project::Utilities::MathUtils;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  EntitySeeder::EntitySeeder(EntitiesManager& mgr, Project::Factories::EntitiesFactory& fac)
  : manager(mgr), factory(fac) {
    std::random_device rd;
    rng.seed(rd());
  }

  void EntitySeeder::setPlayer(std::shared_ptr<Entity> p) {
    player = p;
  }

  void EntitySeeder::addEntityTemplate(const std::string& name) {
    entityTemplates.push_back(name);
  }

  float EntitySeeder::distanceSquared(float x1, float y1, float x2, float y2) const {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return MathUtils::dot(dx, dx, dy, dy);
  }

  void EntitySeeder::spawnEntity() {
    if (entityTemplates.empty()) return;
    auto p = player.lock();
    if (!p) return;

    std::string tmpl = entityTemplates[idCounter % entityTemplates.size()];
    std::unique_ptr<Entity> entity = factory.cloneEntity(tmpl);
    if (!entity) return;

    float px = p->getX();
    float py = p->getY();

    float angle = dist(rng) * Constants::ANGLE_TWO_PI_RAD;
    float radius = spawnRadius * (Constants::DEFAULT_HALF + Constants::DEFAULT_HALF * dist(rng));
    float ex = px + std::cos(angle) * radius;
    float ey = py + std::sin(angle) * radius;

    entity->getLuaStateWrapper().setGlobalNumber(Keys::X, ex);
    entity->getLuaStateWrapper().setGlobalNumber(Keys::Y, ey);
    entity->initialize();

    std::shared_ptr<Entity> shared = std::move(entity);
    std::string id = tmpl + std::string(Constants::SEED) + std::to_string(idCounter++);
    manager.addEntity(id, shared);
    spawnedIds.push_back(id);
  }

  void EntitySeeder::update(float deltaTime) {
    auto p = player.lock();
    if (!p) return;
    float px = p->getX();
    float py = p->getY();

    auto it = spawnedIds.begin();
    while (it != spawnedIds.end()) {
      auto ent = manager.getEntity(*it);
      if (!ent) {
        it = spawnedIds.erase(it);
        continue;
      }
      float ex = ent->getX();
      float ey = ent->getY();
      if (distanceSquared(px, py, ex, ey) > spawnRadius * spawnRadius * 4.0f) {
        manager.removeEntity(*it);
        it = spawnedIds.erase(it);
      } else {
        ++it;
      }
    }

    spawnEntity();
  }
}

