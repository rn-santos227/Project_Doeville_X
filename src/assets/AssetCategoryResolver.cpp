#include "AssetCategoryResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/AssetCategories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Assets {
  namespace Assets = Project::Libraries::Categories::Assets;

  AssetCategory AssetCategoryResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, AssetCategory>, Assets::ASSET_NAME_ALIAS_COUNT> map{{
      {Assets::AUDIO, AssetCategory::AUDIO},
      {Assets::BGM, AssetCategory::BGM},
      {Assets::FONT, AssetCategory::FONT},
      {Assets::MAP, AssetCategory::MAP},
      {Assets::FONT, AssetCategory::FONT},
      {Assets::TILE, AssetCategory::TILE},
      {Assets::TEXTURE, AssetCategory::TEXTURE}
    }};
    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return AssetCategory::UNKNOWN;
  }
}
