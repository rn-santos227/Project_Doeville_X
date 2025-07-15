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

  };
}

#endif
