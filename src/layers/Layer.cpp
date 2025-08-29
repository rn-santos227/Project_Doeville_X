#include "Layer.h"

#include <algorithm>

#include "libraries/constants/Constants.h"
#include "components/vision_component/VisionComponent.h"

namespace Project::Layers {
  namespace Constants = Project::Libraries::Constants;

  Layer::Layer(const std::string& name, LayerCategory category)
    : Layer(name, category, std::make_shared<Project::Entities::EntitiesManager>()) {}

  Layer::Layer(const std::string& layerName,  LayerCategory category, std::shared_ptr<Project::Entities::EntitiesManager> entitiesManager)
    : name(layerName), category(category), visible(true), active(true), entitiesManager(std::move(entitiesManager)) {
    switch (category) {
      case LayerCategory::HUD:
        followCamera = false;
        interactable = true;
        break;
      
      case LayerCategory::OVERLAY:
        followCamera = false;
        interactable = false;
        break;
      
      default:
        followCamera = true;
        interactable = true;
        break;
    }
  }

  void Layer::update(float deltaTime) {
    if (active && entitiesManager) {
      entitiesManager->update(deltaTime);
    }
  }

  void Layer::render() {
    if (visible && entitiesManager) {
      entitiesManager->render();
      if (renderer && darkness > Constants::ANGLE_0_DEG) {
        int w = 0, h = 0;
        SDL_GetRendererOutputSize(renderer, &w, &h);
        SDL_Texture* mask = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        if (mask) {
          SDL_SetTextureBlendMode(mask, SDL_BLENDMODE_BLEND);
          SDL_SetRenderTarget(renderer, mask);
          SDL_SetRenderDrawColor(renderer, Constants::COLOR_BLACK.r, Constants::COLOR_BLACK.g, Constants::COLOR_BLACK.b, static_cast<Uint8>(darkness * Constants::FLOAT_255));
          SDL_RenderClear(renderer);

          auto& comps = entitiesManager->getComponentArray(Project::Components::ComponentType::VISION);
          for (auto* base : comps) {
            auto* vision = dynamic_cast<Project::Components::VisionComponent*>(base);
            if (vision) vision->renderMask(renderer);
          }

          SDL_SetRenderTarget(renderer, nullptr);
          SDL_RenderCopy(renderer, mask, nullptr, nullptr);
          SDL_DestroyTexture(mask);
        }
      }
    }
  }

  void Layer::setDarkness(float value) {
    darkness = std::clamp(value, Constants::ANGLE_0_DEG, Constants::DEFAULT_WHOLE);
  }
}
