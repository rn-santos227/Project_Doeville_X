#ifndef PROFILE_CONSTANTS_H
#define PROFILE_CONSTANTS_H

#include <SDL.h>

namespace Project::Libraries::Constants {
  constexpr const char* BEHAVIOR_PROFILE = "BehaviorSystem::update";
  constexpr const char* MOTION_PROFILE = "MotionSystem::update";
  constexpr const char* PHYSICS_PROFILE = "PhysicsSystem::update";
  constexpr const char* RENDER_PROFILE = "RenderSystem::render";

  constexpr const char* RENDER_SCOPE = "render";
  constexpr const char* POST_PROCESS_SCOPE = "postprocess";
  constexpr const char* PARTICLE_SCOPE = "particles";
}

#endif
