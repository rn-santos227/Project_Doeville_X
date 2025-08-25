#include "DimensionModeResolver.h"

#include <array>
#include <string_view>

#include "libraries/modes/DimensionModes.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/string/StringUtils.h"

namespace Project::States {
  namespace Dimensions = Project::Libraries::Modes::Dimensions;
  namespace Constants = Project::Libraries::Constants;

  DimensionMode DimensionModeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, DimensionMode>, Constants::DIMENSION_NAME_ALIAS_COUNT> map{{
      {Dimensions::BOXED, DimensionMode::BOXED},
      {Dimensions::BOUNDED, DimensionMode::BOUNDED},
      {Dimensions::FREE_ROAMING, DimensionMode::FREE_ROAMING},
      {Dimensions::HORIZONTAL_SCROLL, DimensionMode::HORIZONTAL_SCROLL},
      {Dimensions::ISOMETRIC, DimensionMode::ISOMETRIC},
      {Dimensions::LOOPING, DimensionMode::LOOPING},
      {Dimensions::MAPPED, DimensionMode::MAPPED},
      {Dimensions::THREE_DIMENSION, DimensionMode::THREE_DIMENSION},
      {Dimensions::VERTICAL_SCROLL, DimensionMode::VERTICAL_SCROLL},
      {Dimensions::WRAPPING, DimensionMode::WRAPPING}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return DimensionMode::BOXED;
  }
}
