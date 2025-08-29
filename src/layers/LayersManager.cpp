#include "LayersManager.h"

#include <algorithm>

#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"

namespace Project::Layers {
  namespace Layers = Project::Libraries::Categories::Layers;
  namespace Constants = Project::Libraries::Constants;

  void LayersManager::update(float deltaTime) {
    bool cinematicOnly = hasActiveCinematic();
    for (auto& layer : layers) {
      if (cinematicOnly && layer.getCategory() != LayerCategory::CINEMATIC) {
        continue;
      }
      layer.update(deltaTime);
    }
  }

  void LayersManager::render() {
    bool cinematicOnly = hasActiveCinematic();
    for (auto& layer : layers) {
      if (cinematicOnly && layer.getCategory() != LayerCategory::CINEMATIC) {
        continue;
      }
      layer.render();
    }
  }

  void LayersManager::reset() {
    for (auto& layer : layers) {
      auto mgr = layer.getEntitiesManager();
      if (mgr) {
        mgr->reset();
      }
    }
  }

  void LayersManager::setLogsManager(Project::Utilities::LogsManager* manager) {
    logsManager = manager;
    for (auto& layer : layers) {
      auto mgr = layer.getEntitiesManager();
      if (mgr) {
        mgr->setLogsManager(manager);
      }
    }
  }

  void LayersManager::addLayer(Layer layer) {
    if (logsManager && layer.getEntitiesManager()) {
      layer.getEntitiesManager()->setLogsManager(logsManager);
    }
    if (layer.getCategory() == LayerCategory::CUSTOM) {
      layers.emplace_back(std::move(layer));
      return;
    }

    int order = categoryOrder(layer.getCategory());
    auto it = layers.begin();
    for (; it != layers.end(); ++it) {
      if (it->getCategory() == LayerCategory::CUSTOM) continue;
      if (categoryOrder(it->getCategory()) > order) break;
    }

    layers.emplace(it, std::move(layer));
  }

  void LayersManager::addLayer(const std::string& name, LayerCategory category) {
    if (category == LayerCategory::CUSTOM) {
      layers.emplace_back(name, category);
      if (logsManager)
        layers.back().getEntitiesManager()->setLogsManager(logsManager);
      return;
    }

    int order = categoryOrder(category);
    auto it = layers.begin();
    for (; it != layers.end(); ++it) {
      if (it->getCategory() == LayerCategory::CUSTOM) continue;
      if (categoryOrder(it->getCategory()) > order) break;
    }

    auto inserted = layers.emplace(it, name, category);
    if (logsManager)
      inserted->getEntitiesManager()->setLogsManager(logsManager);
  }

  void LayersManager::addLayer(LayerCategory category) {
    std::string name;
    switch (category) {
      case LayerCategory::HUD: name = std::string(Layers::HUD); break;
      case LayerCategory::OVERLAY: name = std::string(Layers::OVERLAY); break;
      case LayerCategory::FOREGROUND: name = std::string(Layers::FOREGROUND); break;
      case LayerCategory::MIDGROUND: name = std::string(Layers::MIDGROUND); break;
      case LayerCategory::BACKGROUND: name = std::string(Layers::BACKGROUND); break;
      case LayerCategory::CINEMATIC: name = std::string(Layers::CINEMATIC); break;
      case LayerCategory::CUSTOM: name = std::string(Constants::EMPTY_STRING); break;
    }
    addLayer(name, category);
  }

  bool LayersManager::hasLayer(const std::string& name) const {
    return std::any_of(layers.begin(), layers.end(), [&name](const Layer& l){ return l.getName() == name; });
  }

  bool LayersManager::hasLayer(LayerCategory category) const {
    return std::any_of(layers.begin(), layers.end(), [category](const Layer& l){ return l.getCategory() == category; });
  }

  std::shared_ptr<Project::Entities::EntitiesManager> LayersManager::getLayer(const std::string& name) {
    for (auto& layer : layers) {
      if (layer.getName() == name) {
        return layer.getEntitiesManager();
      }
    }
    return nullptr;
  }

  std::shared_ptr<Project::Entities::EntitiesManager> LayersManager::getLayer(LayerCategory category) {
    for (auto& layer : layers) {
      if (layer.getCategory() == category) {
        return layer.getEntitiesManager();
      }
    }
    return nullptr;
  }

  std::shared_ptr<Project::Entities::EntitiesManager> LayersManager::getFirstLayer() {
    if (!layers.empty()) {
      return layers.front().getEntitiesManager();
    }
    return nullptr;
  }

  std::shared_ptr<Project::Entities::EntitiesManager> LayersManager::getLastLayer() {
    if (!layers.empty()) {
      return layers.back().getEntitiesManager();
    }
    return nullptr;
  }

  void LayersManager::removeLayer(const std::string& name) {
    layers.erase(std::remove_if(layers.begin(), layers.end(), [&name](const Layer& l){ return l.getName() == name; }), layers.end());
  }

  void LayersManager::setLayerActive(const std::string& name, bool active) {
    for (auto& layer : layers) {
      if (layer.getName() == name) {
        layer.setActive(active);
        break;
      }
    }
  }

  void LayersManager::setLayerInteractable(const std::string& name, bool active) {
    for (auto& layer : layers) {
      if (layer.getName() == name) {
        layer.setInteractable(active);
        break;
      }
    }
  }

  void LayersManager::setFollowCamera(const std::string& name, bool active) {
    for (auto& layer : layers) {
      if (layer.getName() == name) {
        layer.setFollowCamera(active);
        break;
      }
    }
  }

  void LayersManager::setLayerVisible(const std::string& name, bool visible) {
    for (auto& layer : layers) {
      if (layer.getName() == name) {
        layer.setVisible(visible);
        break;
      }
    }
  }

  void LayersManager::setLayerDarkness(const std::string& name, float value) {
    for (auto& layer : layers) {
      if (layer.getName() == name) {
        layer.setDarkness(value);
        break;
      }
    }
  }

  std::shared_ptr<Project::Entities::Entity> LayersManager::findEntity(const std::string& name) {
    for (auto& layer : layers) {
      auto mgr = layer.getEntitiesManager();
      if (mgr) {
        auto ent = mgr->getEntity(name);
        if (ent) return ent;
      }
    }
    return nullptr;
  }

  size_t LayersManager::getTotalEntityCount() const {
    size_t count = 0;
    for (const auto& layer : layers) {
      auto mgr = layer.getEntitiesManager();
      if (mgr) {
        count += mgr->getEntityCount();
      }
    }
    return count;
  }


  void LayersManager::setGameState(Project::States::GameState* state) {
    gameState = state;
    for (auto& layer : layers) {
      auto mgr = layer.getEntitiesManager();
      if (mgr) mgr->setGameState(state);
    }
  }

  void LayersManager::clampEntitiesToRect(const SDL_Rect& rect) {
    for (auto& layer : layers) {
      auto mgr = layer.getEntitiesManager();
      if (mgr) mgr->clampEntitiesToRect(rect);
    }
  }

  int LayersManager::categoryOrder(LayerCategory category) const {
    switch (category) {
      case LayerCategory::BACKGROUND: return Constants::INDEX_ZERO;
      case LayerCategory::MIDGROUND: return Constants::INDEX_ONE;
      case LayerCategory::FOREGROUND: return Constants::INDEX_TWO;
      case LayerCategory::OVERLAY: return Constants::INDEX_THREE;
      case LayerCategory::HUD: return Constants::INDEX_FOUR;
      case LayerCategory::CINEMATIC: return Constants::INDEX_FIVE;
      case LayerCategory::CUSTOM: default: return Constants::INDEX_TWO;
    }
  }

  bool LayersManager::hasActiveCinematic() const {
    return std::any_of(layers.begin(), layers.end(), [](const Layer& l){
      return l.getCategory() == LayerCategory::CINEMATIC && l.isActive();
    });
  }
}
