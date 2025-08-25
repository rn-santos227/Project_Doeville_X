#include "ScriptingCategoryResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/Categories.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/string/StringUtils.h"

namespace Project::Services {
  namespace Scripts = Project::Libraries::Categories::Scripts;
  namespace Constants = Project::Libraries::Constants;
  
  ScriptCategory ScriptingCategoryResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, ScriptCategory>, Constants::SCRIPTING_NAME_ALIAS_COUNT> map{{
      {Scripts::ENTITY, ScriptCategory::ENTITY},
      {Scripts::ITEM, ScriptCategory::ITEM},
      {Scripts::ASSET, ScriptCategory::ASSET},
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
      case ScriptCategory::ASSET: return Scripts::ASSET;
      case ScriptCategory::LAYER: return Scripts::LAYER;
      case ScriptCategory::STATE: return Scripts::STATE;
      case ScriptCategory::OTHER: return Scripts::OTHER;
      default: return Scripts::INVALID;
    }
  }
}
