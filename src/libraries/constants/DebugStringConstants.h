#ifndef DEBUG_STRING_CONSTANTS_H
#define DEBUG_STRING_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr const char* DEBUG_FPS_PREFIX = "FPS: ";
  constexpr const char* DEBUG_FRAME_PREFIX = "FRAME: ";
  constexpr const char* DEBUG_FRAME_SUFFIX = " MS";
  constexpr const char* DEBUG_MEM_PREFIX = "MEM: ";
  constexpr const char* DEBUG_MEM_SUFFIX = " MB";
  constexpr const char* DEBUG_PROC_PREFIX = "PROC: ";
  constexpr const char* DEBUG_MOUSE_PREFIX = "Mouse: (";
  constexpr const char* DEBUG_MOUSE_SEPARATOR = ", ";
  constexpr const char* DEBUG_MOUSE_SUFFIX = ")";

  constexpr const char* SEED = "_seed_";
  constexpr const char* ENTITY = "entity";
}

#endif
