#ifndef ENTITY_SEEDER_H
#define ENTITY_SEEDER_H

#include <memory>
#include <string>
#include <vector>
#include <random>

#include "interfaces/update_interface/Updatable.h"

namespace Project::Factories { class EntitiesFactory; }
namespace Project::Entities {
  class Entity;
  class EntitiesManager;

  class EntitySeeder : public Project::Interfaces::Updatable {
  public:
    EntitySeeder(EntitiesManager& manager, Project::Factories::EntitiesFactory& factory);

  private:
    std::weak_ptr<Entity> player;
    EntitiesManager& manager;
    Project::Factories::EntitiesFactory& factory;

    std::vector<std::string> entityTemplates;
    std::vector<std::string> spawnedIds;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist{-1.0f, 1.0f};

    float spawnRadius = 300.0f;
    size_t maxEntities = 10;
    size_t idCounter = 0;

    void spawnEntity();
    float distanceSquared(float x1, float y1, float x2, float y2) const;
  };
}

#endif
