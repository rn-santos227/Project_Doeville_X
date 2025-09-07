#include "LayerCategoryResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/Categories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Layers {
  namespace Layers = Project::Libraries::Categories::Layers;

  LayerCategory LayerCategoryResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, LayerCategory>, Layers::LAYER_NAME_ALIAS_COUNT> map{{
      {Layers::HUD, LayerCategory::HUD},
      {Layers::OVERLAY, LayerCategory::OVERLAY},
      {Layers::FOREGROUND, LayerCategory::FOREGROUND},
      {Layers::MIDGROUND, LayerCategory::MIDGROUND},
      {Layers::BACKGROUND, LayerCategory::BACKGROUND},
      {Layers::CINEMATIC, LayerCategory::CINEMATIC},
      {Layers::CUSTOM, LayerCategory::CUSTOM}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return LayerCategory::CUSTOM;
  }
}
