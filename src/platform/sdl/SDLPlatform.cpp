#include "SDLPlatform.h"

#include "libraries/constants/Constants.h"

namespace Project::Platform {
  using Project::Utilities::LogsManager;
  SDLPlatform::SDLPlatform(LogsManager& logsManager)
    : logsManager(logsManager), rendererAPI(nullptr), 
      window(nullptr), renderer(nullptr), glContext(nullptr), 
      exitRequested(false), initialized(false),
      openGLMode(false), vsyncEnabled(false) {}
}
