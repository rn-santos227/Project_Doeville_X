#include "RenderSystem.h"

#include <algorithm>
#include <unordered_map>

#include "components/graphics_component/GraphicsComponent.h"
#include "handlers/camera/CameraHandler.h"
#include "libraries/constants/Constants.h"

namespace Project::Systems {
  using Project::Components::GraphicsComponent;

  namespace Constants = Project::Libraries::Constants;
  RenderSystem::RenderSystem() {
    components.reserve(Project::Libraries::Constants::MAX_MEMORY_SPACE);
  }

  void Project::Systems::RenderSystem::add(GraphicsComponent* component) {
    if (component) components.push_back(component);
  }

  void Project::Systems::RenderSystem::remove(GraphicsComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void Project::Systems::RenderSystem::update(float deltaTime) {
    for (auto* comp : components) {
      if (comp && comp->isActive()) {
        comp->update(deltaTime);
      }
    }
  }

  void Project::Systems::RenderSystem::render() {
    auto hash = [](int x, int y) { return (static_cast<long long>(x) << Constants::BIT_32) ^ static_cast<unsigned int>(y); };

    std::unordered_map<long long, std::vector<GraphicsComponent*>> grid;
    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      const SDL_Rect rect = comp->getRect();
      int cellX = rect.x / Constants::DEFAULT_RENDER_CELL_SIZE;
      int cellY = rect.y / Constants::DEFAULT_RENDER_CELL_SIZE;
      grid[hash(cellX, cellY)].push_back(comp);
    }

    auto* camHandler = GraphicsComponent::getCameraHandler();
    float zoom = Constants::DEFAULT_CAMERA_ZOOM;
    SDL_Rect viewport{0, 0, 0, 0};

    bool useCull = false;
    std::vector<GraphicsComponent*> candidates;

    if (camHandler) {
      zoom = camHandler->getZoom();
      viewport = SDL_Rect{0, 0, camHandler->getViewportWidth(), camHandler->getViewportHeight()};
      useCull = true;
      int startX = camHandler->getX() / Constants::DEFAULT_RENDER_CELL_SIZE;
      int startY = camHandler->getY() / Constants::DEFAULT_RENDER_CELL_SIZE;
      int endX = (camHandler->getX() + camHandler->getViewportWidth()) / Constants::DEFAULT_RENDER_CELL_SIZE;
      int endY = (camHandler->getY() + camHandler->getViewportHeight()) / Constants::DEFAULT_RENDER_CELL_SIZE;
      for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
          auto it = grid.find(hash(x, y));
          if (it != grid.end()) candidates.insert(candidates.end(), it->second.begin(), it->second.end());
        }
      }
    } else {
      candidates = components;
    }

    std::sort(candidates.begin(), candidates.end(), [](GraphicsComponent* a, GraphicsComponent* b) {
      if (a->getDepth() != b->getDepth()) return a->getDepth() > b->getDepth();
      if (a->getBlendMode() != b->getBlendMode()) return a->getBlendMode() < b->getBlendMode();
      return a->getBatchTexture() < b->getBatchTexture();
    });

    struct DrawnRect { SDL_Rect rect; int depth; };
    std::vector<DrawnRect> drawn;
    drawn.reserve(candidates.size());

    SDL_Texture* currentTex = nullptr;
    SDL_BlendMode currentBlend = SDL_BLENDMODE_INVALID;
    SDL_Renderer* renderer = candidates.empty() ? nullptr : candidates.front()->getRenderer();

    std::vector<SDL_Vertex> instVerts;
    std::vector<int> instIndices;
    instVerts.reserve(candidates.size() * Constants::INDEX_FOUR);
    instIndices.reserve(candidates.size() * Constants::INDEX_SIX);

    auto flush = [&]() {
      if (!instVerts.empty() && renderer && currentTex) {
        SDL_RenderGeometry(renderer, currentTex, instVerts.data(), static_cast<int>(instVerts.size()), instIndices.data(), static_cast<int>(instIndices.size()));
        instVerts.clear();
        instIndices.clear();
      }
    };

    int base = 0;
    int texW = 0, texH = 0;
    for (auto* comp : candidates) {
      if (!comp || !comp->isActive()) continue;
      const SDL_Rect rect = comp->getRect();

      SDL_Rect screenRect = rect;
      if (useCull) {
        screenRect.x = static_cast<int>((rect.x - camHandler->getX()) * zoom);
        screenRect.y = static_cast<int>((rect.y - camHandler->getY()) * zoom);
        screenRect.w = static_cast<int>(rect.w * zoom);
        screenRect.h = static_cast<int>(rect.h * zoom);
        if (!SDL_HasIntersection(&screenRect, &viewport)) continue;
      }

      bool occluded = false;
      for (const auto& r : drawn) {
        if (r.depth >= comp->getDepth() && rectContains(r.rect, screenRect)) { occluded = true; break; }
      }
      if (occluded) continue;

      SDL_Texture* tex = comp->getBatchTexture();
      SDL_BlendMode blend = comp->getBlendMode();
      if (blend != currentBlend) {
        flush();
        currentBlend = blend;
        if (renderer) SDL_SetRenderDrawBlendMode(renderer, blend);
      }
      if (tex != currentTex) {
        flush();
        currentTex = tex;
        if (currentTex) {
          SDL_SetTextureBlendMode(currentTex, blend);
          SDL_QueryTexture(currentTex, nullptr, nullptr, &texW, &texH);
        }
      }

      if (comp->isInstanced() && tex) {
        const SDL_Rect& src = comp->getSrcRect();
        SDL_FRect dst{static_cast<float>(screenRect.x), static_cast<float>(screenRect.y), static_cast<float>(screenRect.w), static_cast<float>(screenRect.h)};
        SDL_Vertex v[4];
        float u1 = src.w > 0 ? static_cast<float>(src.x) / texW : 0.0f;
        float v1 = src.h > 0 ? static_cast<float>(src.y) / texH : 0.0f;
        float u2 = src.w > 0 ? static_cast<float>(src.x + src.w) / texW : 1.0f;
        float v2 = src.h > 0 ? static_cast<float>(src.y + src.h) / texH : 1.0f;
        v[Constants::INDEX_ZERO].position = {dst.x, dst.y};
        v[Constants::INDEX_ZERO].tex_coord = {u1, v1};
        v[Constants::INDEX_ZERO].color = Constants::COLOR_WHITE;
        v[Constants::INDEX_ONE].position = {dst.x + dst.w, dst.y};
        v[Constants::INDEX_ONE].tex_coord = {u2, v1};
        v[Constants::INDEX_ONE].color = Constants::COLOR_WHITE;
        v[Constants::INDEX_TWO].position = {dst.x + dst.w, dst.y + dst.h};
        v[Constants::INDEX_TWO].tex_coord = {u2, v2};
        v[Constants::INDEX_TWO].color = Constants::COLOR_WHITE;
        v[Constants::INDEX_THREE].position = {dst.x, dst.y + dst.h};
        v[Constants::INDEX_THREE].tex_coord = {u1, v2};
        v[Constants::INDEX_THREE].color = Constants::COLOR_WHITE;
        instVerts.insert(instVerts.end(), v, v + Constants::INDEX_FOUR);
        
        int idx[Constants::INDEX_SIX] = {0,1,2,0,2,3};
        for (int i = 0; i < Constants::INDEX_SIX; ++i) instIndices.push_back(base + idx[i]);
        
        base += Constants::INDEX_FOUR;
      } else {
        flush();
        comp->render();
      }

      drawn.push_back({screenRect, comp->getDepth()});
    }
    flush();
  }

  void Project::Systems::RenderSystem::clear() {
    components.clear();
  }

  bool RenderSystem::rectContains(const SDL_Rect& outer, const SDL_Rect& inner) const {
    return inner.x >= outer.x && inner.y >= outer.y &&
           (inner.x + inner.w) <= (outer.x + outer.w) &&
           (inner.y + inner.h) <= (outer.y + outer.h);
  }
}
