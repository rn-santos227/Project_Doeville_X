#include "EntitySeeder.h"

#include <cmath>
#include <algorithm>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/math/MathUtils.h"

namespace Project::Entities {
  using Project::Utilities::MathUtils;

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
}

