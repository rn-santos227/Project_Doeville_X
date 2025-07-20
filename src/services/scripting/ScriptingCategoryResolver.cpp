#include "ScriptingCategoryResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/categories/Categories.h"

namespace Project::Services {
  namespace Scripts = Project::Libraries::Categories::Scripts;
  
  ScriptCategory ScriptingCategoryResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, ScriptCategory> map = {
      {std::string(Scripts::ENTITY), ScriptCategory::ENTITY},
      {std::string(Scripts::ITEM), ScriptCategory::ITEM},
      {std::string(Scripts::ANIMATION), ScriptCategory::ANIMATION},
      {std::string(Scripts::MAP), ScriptCategory::MAP},
      {std::string(Scripts::LAYER), ScriptCategory::LAYER},
      {std::string(Scripts::STATE), ScriptCategory::STATE},
      {std::string(Scripts::OTHER), ScriptCategory::OTHER}
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
      return std::toupper(c);
    });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
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
