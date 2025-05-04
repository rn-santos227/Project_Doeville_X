#include "ComponentFactory.h"

namespace Project::Factories {
  ComponentFactory::ComponentFactory(SDL_Renderer* renderer, LogsManager& logsManager)
  : renderer(renderer), logsManager(logsManager) {}
}