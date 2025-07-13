#include "LayersManager.h"

#include <algorithm>

#include "libraries/categories/Categories.h"

namespace Project::Layers {
  namespace Layers = Project::Libraries::Categories::Layers;

  void LayersManager::addLayer(const std::string& name, LayerCategory category) {
    layers.emplace_back(name, category);
  }

  void LayersManager::addLayer(LayerCategory category) {
    std::string name;
    switch (category) {
      case LayerCategory::HUD: name = "HUD"; break;
      case LayerCategory::OVERLAY: name = "Overlay"; break;
      case LayerCategory::FOREGROUND: name = "Foreground"; break;
      case LayerCategory::MIDGROUND: name = "Midground"; break;
      case LayerCategory::BACKGROUND: name = "Background"; break;
      case LayerCategory::CINEMATIC: name = "Cinematic"; break;
      case LayerCategory::CUSTOM: name = ""; break;
    }
    addLayer(name, category);
  }
}
