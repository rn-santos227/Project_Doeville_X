#include "RenderSystem.h"

#include <algorithm>

#include "components/graphics_component/GraphicsComponent.h"
#include "libraries/constants/Constants.h"
#include "utilities/profiler/Profiler.h"
#include "utilities/thread/ThreadPool.h"

namespace Project::Systems {
  using Project::Components::GraphicsComponent;

  namespace Constants = Project::Libraries::Constants;
  RenderSystem::RenderSystem() {
    components.reserve(Constants::MAX_MEMORY_SPACE);
  }

  void RenderSystem::add(GraphicsComponent* component) {
    if (component) components.push_back(component);
  }

  void RenderSystem::remove(GraphicsComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void RenderSystem::update(float deltaTime) {
    PROFILE_SCOPE(Project::Libraries::Constants::RENDER_PROFILE);
    for (auto* comp : components) {
      if (comp && comp->isActive()) comp->update(deltaTime);
    }
  }

  void Project::Systems::RenderSystem::render() {
    ++currentFrame;
    auto hash = [](int x, int y) {
      return (
        static_cast<std::uint64_t>(static_cast<std::int64_t>(x)) << Constants::BIT_32) ^
        static_cast<std::uint64_t>(static_cast<std::int64_t>(y)
      );
    };

    std::unordered_map<std::uint64_t, std::vector<GraphicsComponent*>> grid;
    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      const SDL_FRect rect = comp->getRect();
      int startCellX = static_cast<int>(rect.x / Constants::DEFAULT_RENDER_CELL_SIZE);
      int startCellY = static_cast<int>(rect.y / Constants::DEFAULT_RENDER_CELL_SIZE);
      int endCellX = static_cast<int>((rect.x + rect.w) / Constants::DEFAULT_RENDER_CELL_SIZE);
      int endCellY = static_cast<int>((rect.y + rect.h) / Constants::DEFAULT_RENDER_CELL_SIZE);
      for (int x = startCellX; x <= endCellX; ++x) {
        for (int y = startCellY; y <= endCellY; ++y) {
          grid[hash(x, y)].push_back(comp);
        }
      }
    }

    auto* camHandler = GraphicsComponent::getCameraHandler();
    float zoom = Constants::DEFAULT_CAMERA_ZOOM;
    SDL_Rect viewport{0, 0, 0, 0};
    Project::Handlers::Camera cam{};

    bool useCull = false;
    std::vector<GraphicsComponent*> candidates;

    if (camHandler) {
      zoom = camHandler->getZoom();
      viewport = SDL_Rect{0, 0, camHandler->getViewportWidth(), camHandler->getViewportHeight()};
      useCull = true;
      int startX = static_cast<int>(camHandler->getX() / Constants::DEFAULT_RENDER_CELL_SIZE);
      int startY = static_cast<int>(camHandler->getY() / Constants::DEFAULT_RENDER_CELL_SIZE);
      int endX = static_cast<int>((camHandler->getX() + camHandler->getViewportWidth()) / Constants::DEFAULT_RENDER_CELL_SIZE);
      int endY = static_cast<int>((camHandler->getY() + camHandler->getViewportHeight()) / Constants::DEFAULT_RENDER_CELL_SIZE);
      std::unordered_set<GraphicsComponent*> unique;
      for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
          auto it = grid.find(hash(x, y));
          if (it != grid.end()) unique.insert(it->second.begin(), it->second.end());
        }
      }
      candidates.assign(unique.begin(), unique.end());
      SDL_FRect fr{
        camHandler->getX(), camHandler->getY(),
        static_cast<float>(camHandler->getViewportWidth()), static_cast<float>(camHandler->getViewportHeight())
      };
    } else {
      candidates = components;
      SDL_FRect fr{
        Constants::DEFAULT_FRECT_X,
        Constants::DEFAULT_FRECT_Y,
        Constants::DEFAULT_FRECT_W,
        Constants::DEFAULT_FRECT_H
      };
      cam = Project::Handlers::makeCameraFromRect(fr);
    }

    std::vector<SDL_FRect> occlusionBuffer;
    for (auto* comp : candidates) {
      if (comp && comp->isActive() && comp->isOccluder() && comp->isInFrustum(cam)) {
        occlusionBuffer.push_back(comp->getBoundingBox());
      }
    }

    std::vector<GraphicsComponent*> visible;
    for (auto* comp : candidates) {
      if (comp && comp->isActive() && comp->isVisible(cam, occlusionBuffer, currentFrame)) {
        visible.push_back(comp);
      }
    }
    candidates.swap(visible);

    std::sort(candidates.begin(), candidates.end(), [](GraphicsComponent* a, GraphicsComponent* b) {
      if (a->getMaterialId() != b->getMaterialId()) return a->getMaterialId() < b->getMaterialId();
      if (a->getBlendMode() != b->getBlendMode()) return a->getBlendMode() < b->getBlendMode();
      return a->getBatchTexture() < b->getBatchTexture();
    });

    struct DrawnRect { SDL_FRect rect; int depth; };
    std::vector<DrawnRect> drawn;
    drawn.reserve(candidates.size());

    SDL_Texture* currentTex = nullptr;
    SDL_BlendMode currentBlend = SDL_BLENDMODE_INVALID;
    std::uint32_t currentMaterial = std::numeric_limits<std::uint32_t>::max();
    SDL_Renderer* renderer = candidates.empty() ? nullptr : candidates.front()->getRenderer();

    std::vector<SDL_Vertex> instVerts;
    std::vector<int> instIndices;
    instVerts.reserve(candidates.size() * Constants::INDEX_FOUR);
    instIndices.reserve(candidates.size() * Constants::INDEX_SIX);

    auto flush = [&]() {
      if (!instVerts.empty() && renderer && currentTex) {
        SDL_RenderGeometry(renderer, currentTex, instVerts.data(), static_cast<int>(instVerts.size()), instIndices.data(), static_cast<int>(instIndices.size()));
        Project::Utilities::Profiler::getInstance().incrementDrawCalls();
        instVerts.clear();
        instIndices.clear();
      }
    };

    int base = 0;
    int texW = 0, texH = 0;
    for (auto* comp : candidates) {
      if (!comp || !comp->isActive()) continue;
      const SDL_FRect rect = comp->getRect();

      SDL_FRect screenRect = rect;
      if (useCull) {
        screenRect.x = (rect.x - camHandler->getX()) * zoom;
        screenRect.y = (rect.y - camHandler->getY()) * zoom;
        screenRect.w = rect.w * zoom;
        screenRect.h = rect.h * zoom;
        
        SDL_FRect viewportF{
          static_cast<float>(viewport.x),
          static_cast<float>(viewport.y),static_cast<float>(viewport.w),static_cast<float>(viewport.h)
        };
        
        if (!SDL_HasIntersectionF(&screenRect, &viewportF)) continue;
      }

      bool occluded = false;
      for (const auto& r : drawn) {
        if (r.depth >= comp->getDepth() && rectContains(r.rect, screenRect)) { occluded = true; break; }
      }
      if (occluded) continue;

      SDL_Texture* tex = comp->getBatchTexture();
      SDL_BlendMode blend = comp->getBlendMode();
      std::uint32_t matId = comp->getMaterialId();

      if (matId != currentMaterial) {
        flush();
        currentMaterial = matId;
      }

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
        SDL_Vertex v[Constants::INDEX_FOUR];

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
        Project::Utilities::Profiler::getInstance().incrementDrawCalls();
      }

      drawn.push_back({screenRect, comp->getDepth()});
    }
    flush();
  }

  void Project::Systems::RenderSystem::clear() {
    components.clear();
  }

  bool RenderSystem::rectContains(const SDL_FRect& outer, const SDL_FRect& inner) const {
    return inner.x >= outer.x && inner.y >= outer.y && (inner.x + inner.w) <= (outer.x + outer.w) && (inner.y + inner.h) <= (outer.y + outer.h);
  }
}
