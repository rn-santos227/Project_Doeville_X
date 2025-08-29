#ifndef PORTAL_DATA_H
#define PORTAL_DATA_H

#include <string>

namespace Project::Components {
  struct PortalData {
    float targetX = 0.0f;
    float targetY = 0.0f;
    float delay = 0.0f;

    bool triggerOnAction = false;
    bool triggerOnInside = false;
    bool triggerOnTouch = false;
    
    std::string animation;
  };
}

#endif
