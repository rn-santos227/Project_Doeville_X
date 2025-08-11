#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <vector>
#include <SDL.h>

#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"

namespace Project { namespace Components { class GraphicsComponent; } }

namespace Project::Systems {
  class RenderSystem : public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {
  public:
    RenderSystem();
    
    void add(Project::Components::GraphicsComponent* component);
    void remove(Project::Components::GraphicsComponent* component);
    
    void update(float deltaTime) override;
    void render() override;
    void clear();

  private:
    std::vector<Project::Components::GraphicsComponent*> components;
    
    int currentFrame = 0;
    bool rectContains(const SDL_Rect& outer, const SDL_Rect& inner) const;
  };
}

#endif
