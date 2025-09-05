#ifndef DEBUG_STRING_CONSTANTS_H
#define DEBUG_STRING_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr const char* DEBUG_FPS_PREFIX = "FPS: ";
  constexpr const char* DEBUG_FRAME_PREFIX = "FRAME: ";
  constexpr const char* DEBUG_FRAME_SUFFIX = " MS";
  constexpr const char* DEBUG_MEM_PREFIX = "MEM: ";
  constexpr const char* DEBUG_MEM_SUFFIX = " MB";
  constexpr const char* DEBUG_DRAW_PREFIX = "DRAW: ";
  constexpr const char* DEBUG_PROC_PREFIX = "PROC: ";
  constexpr const char* DEBUG_UPTIME_PREFIX = "TIME: ";
  constexpr const char* DEBUG_UPTIME_SUFFIX = " S";
  constexpr const char* DEBUG_UPTIME_MIN_SUFFIX = " MIN";
  constexpr const char* DEBUG_UPTIME_HOUR_SUFFIX = " H";
  constexpr const char* DEBUG_MOUSE_PREFIX = "Mouse: (";
  constexpr const char* DEBUG_MOUSE_SEPARATOR = ", ";
  constexpr const char* DEBUG_MOUSE_SUFFIX = "cpu_frame";

  constexpr const char* DEBUG_CPU_PREFIX = "CPU: ";
  constexpr const char* DEBUG_GPU_PREFIX = "GPU: ";
  constexpr const char* DEBUG_OCC_PREFIX = "OCC: ";

  constexpr const char* PROFILE_LOD = "lod_before";
  constexpr const char* CPU_FRAME = ")";

  constexpr const char* SEED = "_seed_";
  constexpr const char* ENTITY = "entity";
}

#endif
