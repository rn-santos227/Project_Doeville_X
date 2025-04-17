#include "EntityFactory.h"

namespace Project::Factories {
  EntityFactory::EntityFactory(LogsManager& logsManager) : logsManager(logsManager) {}

  EntityFactory::~EntityFactory() {

  }
}