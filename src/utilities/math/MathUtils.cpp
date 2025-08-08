#include "MathUtils.h"

#include <algorithm>
#include <cmath>

#include "libraries/constants/AngleConstants.h"
#include "libraries/constants/FloatConstants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;

  float MathUtils::angleBetween(float x1, float y1, float x2, float y2) {
    float dotProd = dot(x1, y1, x2, y2);
    float mag1 = magnitude(x1, y1);
    float mag2 = magnitude(x2, y2);
    float denom = mag1 * mag2;
    if (denom == 0.0f) {
      return 0.0f;
    }
    float cosTheta = dotProd / denom;
    cosTheta = std::clamp(cosTheta, -Constants::DEFAULT_WHOLE, Constants::DEFAULT_WHOLE);
    return std::acos(cosTheta);
  }

  float MathUtils::cross(float x1, float y1, float x2, float y2) {
    return x1 * y2 - y1 * x2;
  }

  float MathUtils::dot(float x1, float y1, float x2, float y2) {
    return x1 * x2 + y1 * y2;
  }

  float MathUtils::lerp(float a, float b, float t) {
    return a + t * (b - a);
  }

  float MathUtils::magnitude(float x, float y) {
    return std::sqrt(x * x + y * y);
  }

  int MathUtils::interpolate(int current, int target, float t) {
    if (current == target) {
      return current;
    }
    float result = lerp(static_cast<float>(current), static_cast<float>(target), t);
    int rounded = static_cast<int>(std::round(result));
    if (rounded == current) {
      return current + (target > current ? 1 : -1);
    }
    return rounded;
  }

  SDL_FPoint MathUtils::normalize(float x, float y) {
    float mag = magnitude(x, y);
    if (mag == 0.0f) {
      return {0.0f, 0.0f};
    }
    return {x / mag, y / mag};
  }

  SDL_FPoint MathUtils::rotate(float x, float y, float angleDeg) {
    float rad = angleDeg * Project::Libraries::Constants::DEG_TO_RAD;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);
    return {x * cosA - y * sinA, x * sinA + y * cosA};
  }
}
