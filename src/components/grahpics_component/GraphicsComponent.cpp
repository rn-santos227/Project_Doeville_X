#include "GraphicsComponent.h"
#include <SDL_image.h>

namespace Project::Components {
  GraphicsComponent::GraphicsComponent(SDL_Renderer* renderer, const std::string& texturePath, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), texture(nullptr) {

  }

  GraphicsComponent::~GraphicsComponent() {
    if (texture) {

    }
  }
}