#include "MathUtils.h"

#include <cmath>

namespace Project::Utilities {
  float MathUtils::dot(float x1, float y1, float x2, float y2) {
    return x1 * x2 + y1 * y2;
  }

  float MathUtils::cross(float x1, float y1, float x2, float y2) {
    return x1 * y2 - y1 * x2;
  }

  float MathUtils::magnitude(float x, float y) {
    return std::sqrt(x * x + y * y);
  }

}
