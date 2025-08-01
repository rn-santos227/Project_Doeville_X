#include "ScriptingCategoryResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/Categories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Services {
  namespace Scripts = Project::Libraries::Categories::Scripts;
  
  ScriptCategory ScriptingCategoryResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, ScriptCategory>, 7> map{{
      {Scripts::ENTITY, ScriptCategory::ENTITY},
      {Scripts::ITEM, ScriptCategory::ITEM},
      {Scripts::ANIMATION, ScriptCategory::ANIMATION},
      {Scripts::MAP, ScriptCategory::MAP},
      {Scripts::LAYER, ScriptCategory::LAYER},
      {Scripts::STATE, ScriptCategory::STATE},
      {Scripts::OTHER, ScriptCategory::OTHER}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return ScriptCategory::OTHER;
  }

  std::string ScriptingCategoryResolver::categoryToString(ScriptCategory category) {
    switch(category) {
      case ScriptCategory::ENTITY: return Scripts::ENTITY;
      case ScriptCategory::ITEM: return Scripts::ITEM;
      case ScriptCategory::ANIMATION: return Scripts::ANIMATION;
      case ScriptCategory::MAP: return Scripts::MAP;
      case ScriptCategory::LAYER: return Scripts::LAYER;
      case ScriptCategory::STATE: return Scripts::STATE;
      case ScriptCategory::OTHER: return Scripts::OTHER;
      default: return Scripts::INVALID;
    }
  }
}
