#include "ColorUtils.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Project::Utilities {
  SDL_Color hexToRGB(const std::string& hex, Uint8 alpha) {
    std::string value = hex;
    if (!value.empty() && value[0] == '#') {
      value.erase(0, 1);
    }

    if (value.rfind("0x", 0) == 0 || value.rfind("0X", 0) == 0) {
      value.erase(0, 2);
    }

    if (value.length() != 6) {
      return SDL_Color{0, 0, 0, alpha};
    }

    std::transform(value.begin(), value.end(), value.begin(), ::tolower);

    unsigned int rgb;
    std::stringstream ss;
    ss << std::hex << value;
    ss >> rgb;
  }
}
