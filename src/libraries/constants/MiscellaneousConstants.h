#ifndef MISCELLANEOUS_CONSTANTS_H
#define MISCELLANEOUS_CONSTANTS_H

#include <cstdint>
#include <SDL.h>

namespace Project::Libraries::Constants {
  constexpr int SEEDER_KEY = 32;
  constexpr double TARGET_FPS = 60.0;
  constexpr double DEFAULT_MAX_FPS = 120.0;
  
  constexpr size_t BYTES_PER_MEGABYTE = 1024 * 1024;
  constexpr size_t ENTITY_OPTIMIZATION_THRESHOLD = 200;

  constexpr std::uint64_t DEFAULT_HASH = 0x9e3779b97f4a7c15ULL;
  constexpr Uint8 FULL_ALPHA = 255;
}

#endif
