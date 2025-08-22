#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <vector>
#include <SDL.h>

#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"
#include "libraries/constants/IndexConstants.h"

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
    std::vector<Project::Components::GraphicsComponent*> commandBuffers[Project::Libraries::Constants::INDEX_TWO];
    int readIndex = 0;
    int writeIndex = Project::Libraries::Constants::INDEX_ONE;
    int currentFrame = 0;
    
    bool firstFrame = true;

    bool rectContains(const SDL_FRect& outer, const SDL_FRect& inner) const;
    void prepareCommandBuffer(std::vector<Project::Components::GraphicsComponent*>& buffer);
    void drawBuffer(const std::vector<Project::Components::GraphicsComponent*>& buffer);
  };
}

#endif
