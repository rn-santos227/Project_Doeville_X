#ifndef TRANSFORM_DATA_H
#define TRANSFORM_DATA_H

#include <vector>
#include <SDL.h>

#include "libraries/constants/Constants.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Components {

  struct TransformData {
    float flexibility = Project::Libraries::Constants::DEFAULT_FLEXIBILITY;
    float spin = Project::Libraries::Constants::DEFAULT_SPIN;
    bool allowRevert = true;
    bool transformed = false;

    std::vector<SDL_FRect> originalBoxes;
    std::vector<Project::Utilities::Circle> originalCircles;

    void set(float newFlexibility, float newSpin, bool canRevert) {
      flexibility = newFlexibility;
      spin = newSpin;
      allowRevert = canRevert;
    }

    void reset() {
      transformed = false;
      originalBoxes.clear();
      originalCircles.clear();
    }

    void applyTransformationState(bool state) {
      transformed = state;
    }

    bool isTransformed() const {
      return transformed;
    }
  };

}

#endif