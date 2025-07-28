#include "MovementModeResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/modes/MovementModes.h"

namespace Project::Components {
  namespace Movements = Project::Libraries::Modes::Movements;
  MovementMode SurfaceTypeResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, MovementMode> map = {
      {std::string(Movements::STANDARD), MovementMode::STANDARD},
      {std::string(Movements::FLYING), MovementMode::FLYING},
      {std::string(Movements::SCROLLER), MovementMode::SCROLLER},
      {std::string(Movements::VEHICLE), MovementMode::VEHICLE},
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
      return std::toupper(c);
    });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return MovementMode::STANDARD;
  }
}
