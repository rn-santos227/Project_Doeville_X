#include "BackgroundLoader.h"

#include <fstream>
#include <iterator>

#include "libraries/constants/IndexConstants.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Utilities::MemorySystem;
  using Project::Utilities::MemoryBudgetTracker;
  using Project::Utilities::PoolAllocator;
  using Project::Utilities::TextureUtils::compress;
  using Project::Utilities::TextureUtils::selectMip;

  BackgroundLoader::BackgroundLoader(LogsManager& logsManager, MemoryBudgetTracker& budgetTracker, PoolAllocator& pool)
    : logsManager(logsManager), budgetTracker(budgetTracker), pool(pool) {}

  BackgroundLoader::~BackgroundLoader() = default;

  std::future<SDL_Texture*> BackgroundLoader::streamTexture(SDL_Renderer* renderer, const std::string& path, int maxDim) {
    return std::async(std::launch::async, [this, renderer, path, maxDim]() {
      SDL_Surface* surface = IMG_Load(path.c_str());
      if (!surface) {
        logsManager.logError(std::string("Failed to load texture: ") + path + " - " + IMG_GetError());
        return static_cast<SDL_Texture*>(nullptr);
      }
      surface = compress(surface);
      surface = selectMip(surface, maxDim);
      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
      std::size_t mem = surface ? static_cast<std::size_t>(surface->w * surface->h * Project::Libraries::Constants::INDEX_FOUR) : 0u;
      SDL_FreeSurface(surface);
      budgetTracker.allocate(MemorySystem::Textures, mem);
      return texture;
    });
  }

  std::future<MeshData> BackgroundLoader::streamMesh(const std::string& path) {
    return std::async(std::launch::async, [this, path]() {
      MeshData data;
      std::ifstream file(path, std::ios::binary | std::ios::ate);
      if (file) {
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        if (fileSize > 0 && static_cast<std::size_t>(fileSize) <= pool.getBlockSize()) {
          void* block = pool.acquire();
          if (block && file.read(reinterpret_cast<char*>(block), fileSize)) {
            data.size = static_cast<std::size_t>(fileSize);
            data.data.reset(static_cast<unsigned char*>(block), [this](unsigned char* p){ pool.release(p); });
            budgetTracker.allocate(MemorySystem::Meshes, data.size);
          } else if (block) {
            pool.release(block);
            logsManager.logError("Failed to read mesh: " + path);
          }
        } else {
          logsManager.logError("Mesh too large or empty: " + path);
        }
      } else {
        logsManager.logError("Failed to load mesh: " + path);
      }
      return data;
    });
  }

  std::future<AudioData> BackgroundLoader::streamAudio(const std::string& path) {
    return std::async(std::launch::async, [this, path]() {
      AudioData audio;
      if (!SDL_LoadWAV(path.c_str(), &audio.spec, &audio.buffer, &audio.length)) {
        logsManager.logError(std::string("Failed to load audio: ") + path + " - " + SDL_GetError());
        return audio;
      }
      budgetTracker.allocate(MemorySystem::Audio, audio.length);
      return audio;
    });
  }
}
