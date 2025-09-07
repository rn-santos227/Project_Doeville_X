#ifndef SURFACE_CATEGORIES_H
#define SURFACE_CATEGORIES_H

namespace Project::Libraries::Categories {
  namespace Surfaces {
    constexpr size_t SURFACE_NAME_ALIAS_COUNT = 7;
    
    constexpr const char* BOUNCE = "BOUNCE";
    constexpr const char* DESTROY_ON_HIT = "DESTROY_ON_HIT";
    constexpr const char* GHOST_PASS = "GHOST_PASS";
    constexpr const char* REST = "REST";
    constexpr const char* SLIDE = "SLIDE";
    constexpr const char* STICK = "STICK";
    constexpr const char* TRIGGER_EVENT = "TRIGGER_EVENT";
  }
}

#endif
