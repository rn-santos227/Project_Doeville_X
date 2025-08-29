#include "ColorUtils.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Project::Utilities {
  SDL_Color ColorUtils::hexToRGB(const std::string& hex, Uint8 alpha) {
    constexpr Uint8 SIXTEEN_BIT = 16;
    constexpr Uint8 EIGHT_BIT = 8;

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

    SDL_Color color;
    color.r = static_cast<Uint8>((rgb >> SIXTEEN_BIT) & 0xFF);
    color.g = static_cast<Uint8>((rgb >> EIGHT_BIT) & 0xFF);
    color.b = static_cast<Uint8>(rgb & 0xFF);
    color.a = alpha;
    return color;
  }

  SDL_Color ColorUtils::negative(SDL_Color color) {
    constexpr Uint8 MAX_CHANNEL = Project::Libraries::Constants::FULL_ALPHA;
    color.r = static_cast<Uint8>(MAX_CHANNEL - color.r);
    color.g = static_cast<Uint8>(MAX_CHANNEL - color.g);
    color.b = static_cast<Uint8>(MAX_CHANNEL - color.b);
    return color;
  }

  bool ColorUtils::applyShader(SDL_Texture* texture, SDL_Color color) {
    if (!texture) {
      return false;
    }
    if (SDL_SetTextureColorMod(texture, color.r, color.g, color.b) != 0) {
      return false;
    }
    if (SDL_SetTextureAlphaMod(texture, color.a) != 0) {
      return false;
    }
    return true;
  }
}
