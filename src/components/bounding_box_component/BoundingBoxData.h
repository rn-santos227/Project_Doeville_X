#ifndef BOUNDING_BOX_DATA_H
#define BOUNDING_BOX_DATA_H

#include "SurfaceType.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <SDL.h>

#include "utilities/geometry/GeometryUtils.h"
#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct BoundingBoxData {
    SurfaceType surfaceType = SurfaceType::REST;

    std::vector<SDL_FRect> boxes;
    std::vector<Project::Utilities::Circle> circles;
    std::vector<Project::Utilities::OrientedBox> orientedBoxes;
    std::vector<Project::Utilities::Polygon> polygons;
    std::vector<Project::Utilities::Capsule> capsules;

    std::unordered_set<std::string> ignoredEntities;

    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;
    float rotation = Project::Libraries::Constants::DEFAULT_ROTATION;

    bool solid = false;
    bool rotationEnabled = false;

    SDL_Color debugColor = Project::Libraries::Constants::DEFAULT_DEBUG_TEXT_COLOR;
  };
}

#endif
