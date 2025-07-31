#include "DimensionModeResolver.h"

#include <cctype>
#include <unordered_map>

#include "libraries/modes/DimensionModes.h"

namespace Project::States {
  namespace Dimensions = Project::Libraries::Modes::Dimensions;
  DimensionMode DimensionModeResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, DimensionMode> map = {
      {std::string(Dimensions::BOXED), DimensionMode::BOXED},
      {std::string(Dimensions::BOUNDED), DimensionMode::BOUNDED},
      {std::string(Dimensions::FREE_ROAMING), DimensionMode::FREE_ROAMING},
      {std::string(Dimensions::HORIZONTAL_SCROLL), DimensionMode::HORIZONTAL_SCROLL},
      {std::string(Dimensions::ISOMETRIC), DimensionMode::ISOMETRIC},
      {std::string(Dimensions::LOOPING), DimensionMode::LOOPING},
      {std::string(Dimensions::THREE_DIMENSION), DimensionMode::THREE_DIMENSION},
      {std::string(Dimensions::VERTICAL_SCROLL), DimensionMode::VERTICAL_SCROLL},
      {std::string(Dimensions::WRAPPING), DimensionMode::WRAPPING}
    };
  }
}
