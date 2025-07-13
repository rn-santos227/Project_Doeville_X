#include "LayerCategoryResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/categories/Categories.h"

namespace Project::Layers {
  namespace Layers = Project::Libraries::Categories::Layers;
  LayerCategory LayerCategoryResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, LayerCategory> map = {
      {std::string(Layers::HUD), LayerCategory::HUD},
      {std::string(Layers::OVERLAY), LayerCategory::OVERLAY},
      {std::string(Layers::FOREGROUND), LayerCategory::FOREGROUND},
      {std::string(Layers::MIDGROUND), LayerCategory::MIDGROUND},
      {std::string(Layers::BACKGROUND), LayerCategory::BACKGROUND},
      {std::string(Layers::CINEMATIC), LayerCategory::CINEMATIC},
      {std::string(Layers::CUSTOM), LayerCategory::CUSTOM}
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
      return std::tolower(c);
    });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return LayerCategory::CUSTOM;
  }
}
