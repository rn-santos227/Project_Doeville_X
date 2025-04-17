#include "EntityFactory.h"

namespace Project::Factories {
  EntityFactory::EntityFactory(LogsManager& logsManager) : logsManager(logsManager) {}

  EntityFactory::~EntityFactory() {
    entityTemplates.clear();
  }

  bool EntityFactory::createEntityFromLua(const std::string& scriptPath) {

  }

  std::unique_ptr<Entity> EntityFactory::loadEntityTemplateFromLua(const std::string& scriptPath) {

  }
}