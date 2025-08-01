#include "MovementModeResolver.h"

#include <array>
#include <string_view>

#include "libraries/modes/MovementModes.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  namespace Movements = Project::Libraries::Modes::Movements;
  MovementMode MovementModeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, MovementMode>, 4> map{{
      {Movements::STANDARD, MovementMode::STANDARD},
      {Movements::FLYING, MovementMode::FLYING},
      {Movements::SCROLLER, MovementMode::SCROLLER},
      {Movements::VEHICLE, MovementMode::VEHICLE},
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return MovementMode::STANDARD;
  }
}
