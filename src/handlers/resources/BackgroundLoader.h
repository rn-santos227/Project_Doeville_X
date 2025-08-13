#ifndef BACKGROUND_LOADER_H
#define BACKGROUND_LOADER_H

#include <future>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/memory/MemoryBudgetTracker.h"
#include "utilities/texture/TextureUtils.h"
#include "libraries/constants/NumericConstants.h"

namespace Project::Handlers {
  struct MeshData {
    std::vector<char> data;
  };

  struct AudioData {
    SDL_AudioSpec spec{};
    Uint8* buffer{nullptr};
    Uint32 length{0};
  };

  class BackgroundLoader {
  public:
    BackgroundLoader(Project::Utilities::LogsManager& logsManager, Project::Utilities::MemoryBudgetTracker& budgetTracker);
    ~BackgroundLoader();

    std::future<SDL_Texture*> streamTexture(SDL_Renderer* renderer, const std::string& path, int maxDim = Project::Libraries::Constants::DEFAULT_MAX_DIM);
    std::future<MeshData> streamMesh(const std::string& path);
    std::future<AudioData> streamAudio(const std::string& path);

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::MemoryBudgetTracker& budgetTracker;
  };
}

#endif
