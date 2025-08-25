#ifndef LOS_COMPONENT_H
#define LOS_COMPONENT_H

#include "LoSData.h"

#include <vector>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "utilities/logs_manager/LogsManager.h"
#include "entities/EntitiesManager.h"

namespace Project::Components {
  class LoSComponent : public BaseComponent, public PositionableComponent {
  public:
    LoSComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager);

  private:
    SDL_Renderer* renderer;
    Project::Entities::EntitiesManager* entitiesManager = nullptr;
    Project::Entities::Entity* owner = nullptr;
    LoSData data;

    std::vector<SDL_FPoint> endpoints;
    std::vector<Project::Entities::Entity*> visibleEntities;

    void castRays();
    std::pair<SDL_FPoint, Project::Entities::Entity*> castSingleRay(float angleDegrees);
  };
}

#endif
