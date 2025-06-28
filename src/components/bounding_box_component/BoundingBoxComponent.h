#ifndef BOUNDING_BOX_COMPONENT_H
#define BOUNDING_BOX_COMPONENT_H

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"

#include <SDL.h>
#include <vector>

namespace Project::Components {
  class BoundingBoxComponent : public BaseComponent, public PositionableComponent {
  public:
    explicit BoundingBoxComponent(LogsManager& logsManager);

    void update(float deltaTime) override;
    void render() override;

    void addBox(const SDL_Rect& rect);

  private:
    std::vector<SDL_Rect> boxes;
    int entityX = 0;
    int entityY = 0;
    bool solid = false;
  };
}

#endif
