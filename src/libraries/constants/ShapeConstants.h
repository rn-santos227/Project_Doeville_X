#ifndef SHAPE_CONSTANTS_H
#define SHAPE_CONSTANTS_H

#include <array>

#include "IndexConstants.h"

namespace Project::Libraries::Constants {
  constexpr int CIRCLE_DIAMETER_MULTIPLIER = 2;
  constexpr std::array<int, static_cast<std::size_t>(INDEX_SIX)> RECTANGLE_INDICES{
    INDEX_ZERO, INDEX_ONE, INDEX_TWO, INDEX_TWO, INDEX_THREE, INDEX_ZERO
  };
}

#endif
