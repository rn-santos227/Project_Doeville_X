#include "DebugDisplay.h"

#include <cstdio>
#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>

#elif defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <unistd.h>
#include <dirent.h>
#endif

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Utilities::FramesCounter;
  using Project::Utilities::ConfigReader;
  using Project::Core::SDLManager;
  using Project::Handlers::FontHandler;
  using Project::Handlers::MouseHandler;
  
  namespace Constants = Project::Libraries::Constants;

  DebugDisplay::DebugDisplay(LogsManager& logsManager,
    FramesCounter& framesCounter, ConfigReader& configReader,
    SDLManager& sdlManager, FontHandler& fontHandler, MouseHandler& mouseHandler)
    : logsManager(logsManager), framesCounter(framesCounter), configReader(configReader),
      sdlManager(sdlManager), fontHandler(fontHandler), mouseHandler(mouseHandler) {
    debugTextColor = configReader.getColorValue("Debug", "text_color", debugTextColor);
    axisXColor = configReader.getColorValue("Axis", "x_color", axisXColor);
    axisYColor = configReader.getColorValue("Axis", "y_color", axisYColor);
  }

  void DebugDisplay::render() {
    renderGrid();
    renderAxes();
    renderFPS();
    renderBenchmark();
    renderMemoryUsage();
    renderProcessCount();
    renderMousePosition();
  }

  void DebugDisplay::renderFPS() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    int fps = framesCounter.getFPS();
    std::string fpsText = std::string(Constants::DEBUG_FPS_PREFIX) + std::to_string(fps);
    SDL_Texture* fpsTexture = fontHandler.renderText(renderer, fpsText, Constants::DEFAULT_FONT, debugTextColor);

    if (fpsTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(fpsTexture, nullptr, nullptr, &textWidth, &textHeight);

      fpsTextHeight = textHeight;

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {screenWidth - textWidth - Constants::DEBUG_TEXT_MARGIN, Constants::DEBUG_TEXT_MARGIN, textWidth, textHeight};
      SDL_RenderCopy(renderer, fpsTexture, nullptr, &destRect);
      SDL_DestroyTexture(fpsTexture);
    } else {
      logsManager.logError("Failed to render FPS text.");
    }
  }

  void DebugDisplay::renderBenchmark() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    size_t memMB = getProcessMemoryUsageMB();
    std::string memText = std::string(Constants::DEBUG_MEM_PREFIX) + std::to_string(memMB) + Constants::DEBUG_MEM_SUFFIX;
    SDL_Texture* memTexture = fontHandler.renderText(renderer, memText, Constants::DEFAULT_FONT, debugTextColor);

    if (memTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(memTexture, nullptr, nullptr, &textWidth, &textHeight);
      memTextHeight = textHeight;

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {screenWidth - textWidth - Constants::DEBUG_TEXT_MARGIN, Constants::DEBUG_TEXT_MARGIN + fpsTextHeight + Constants::DEBUG_LINE_SPACING, textWidth, textHeight};
      SDL_RenderCopy(renderer, memTexture, nullptr, &destRect);
      SDL_DestroyTexture(memTexture);
    } else {
      logsManager.logError("Failed to render memory usage.");
    }
  }

  void DebugDisplay::renderMemoryUsage() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    size_t memMB = getProcessMemoryUsageMB();
    std::string memText = std::string(Constants::DEBUG_MEM_PREFIX) + std::to_string(memMB) + Constants::DEBUG_MEM_SUFFIX;
    SDL_Texture* memTexture = fontHandler.renderText(renderer, memText, Constants::DEFAULT_FONT, debugTextColor);

    if (memTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(memTexture, nullptr, nullptr, &textWidth, &textHeight);
      memTextHeight = textHeight;

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {screenWidth - textWidth - Constants::DEBUG_TEXT_MARGIN, Constants::DEBUG_TEXT_MARGIN + fpsTextHeight + Constants::DEBUG_LINE_SPACING, textWidth, textHeight};
      SDL_RenderCopy(renderer, memTexture, nullptr, &destRect);
      SDL_DestroyTexture(memTexture);
    } else {
      logsManager.logError("Failed to render memory usage.");
    }
  }

  void DebugDisplay::renderProcessCount() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    size_t procCount = getProcessCount();
    std::string procText = std::string(Constants::DEBUG_PROC_PREFIX) + std::to_string(procCount);
    SDL_Texture* procTexture = fontHandler.renderText(renderer, procText, Constants::DEFAULT_FONT, debugTextColor);

    if (procTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(procTexture, nullptr, nullptr, &textWidth, &textHeight);

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {screenWidth - textWidth - Constants::DEBUG_TEXT_MARGIN, Constants::DEBUG_TEXT_MARGIN + fpsTextHeight + Constants::DEBUG_LINE_SPACING + memTextHeight + Constants::DEBUG_LINE_SPACING, textWidth, textHeight};
      SDL_RenderCopy(renderer, procTexture, nullptr, &destRect);
      SDL_DestroyTexture(procTexture);
    } else {
      logsManager.logError("Failed to render process count.");
    }
  }

  void DebugDisplay::renderAxes() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    // Draw X-axis
    SDL_SetRenderDrawColor(renderer, axisXColor.r, axisXColor.g, axisXColor.b, axisXColor.a);
    SDL_RenderDrawLine(renderer, 0, screenHeight / 2, screenWidth, screenHeight / 2);
    
    // Draw Y-axis
    SDL_SetRenderDrawColor(renderer, axisYColor.r, axisYColor.g, axisYColor.b, axisYColor.a);
    SDL_RenderDrawLine(renderer, screenWidth / 2, 0, screenWidth / 2, screenHeight);
  }

  void DebugDisplay::renderGrid() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    SDL_SetRenderDrawColor(renderer, gridColor.r, gridColor.g, gridColor.b, gridColor.a);

    for (int x = 0; x <= screenWidth; x += gridSpacing) {
      SDL_RenderDrawLine(renderer, x, 0, x, screenHeight);
    }

    for (int y = 0; y <= screenHeight; y += gridSpacing) {
      SDL_RenderDrawLine(renderer, 0, y, screenWidth, y);
    }
  }

  void DebugDisplay::renderMousePosition() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    mouseHandler.updateMousePosition();
    int mouseX = mouseHandler.getMouseX();
    int mouseY = mouseHandler.getMouseY();

    std::string mousePositionText = "Mouse: (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";
    SDL_Texture* mouseTexture = fontHandler.renderText(renderer, mousePositionText, "system", debugTextColor);

    if (mouseTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(mouseTexture, nullptr, nullptr, &textWidth, &textHeight);

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {Constants::DEBUG_TEXT_MARGIN, screenHeight - textHeight - Constants::DEBUG_TEXT_MARGIN, textWidth, textHeight};
      SDL_RenderCopy(renderer, mouseTexture, nullptr, &destRect);
      SDL_DestroyTexture(mouseTexture);
    } else {
      logsManager.logError("Failed to render mouse position text.");
    }
  }

  size_t DebugDisplay::getProcessMemoryUsageMB() {
    #if defined(_WIN32)
      PROCESS_MEMORY_COUNTERS pmc;
      if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<size_t>(pmc.WorkingSetSize) / Constants::BYTES_PER_MEGABYTE;
      }
      return 0;

    #elif defined(__APPLE__) && defined(__MACH__)
      mach_task_basic_info info;
      mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
      if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &infoCount) == KERN_SUCCESS) {
        return static_cast<size_t>(info.resident_size) / Constants::BYTES_PER_MEGABYTE;
      }
      return 0;

    #elif defined(__linux__)
      long rss = 0
      FILE* fp = fopen("/proc/self/statm", "r");
      if (fp) {
        if (fscanf(fp, "%*s%ld", &rss) != 1) {
          rss = 0L;
        }
        fclose(fp);
      }
      long pageSize = sysconf(_SC_PAGESIZE);
      size_t bytes = static_cast<size_t>(rss) * static_cast<size_t>(pageSize);
      return bytes / Constants::BYTES_PER_MEG

    #else
      return 0;

    #endif
  }

  size_t DebugDisplay::getProcessCount() {
    #if defined(_WIN32)
      DWORD processes[1024];
      DWORD bytesNeeded = 0;
      if (EnumProcesses(processes, sizeof(processes), &bytesNeeded)) {
        return bytesNeeded / sizeof(DWORD);
      }
      return 0;

    #elif defined(__APPLE__) && defined(__MACH__)
      int mib[3] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL};
      size_t size = 0;
      if (sysctl(mib, 3, nullptr, &size, nullptr, 0) == 0) {
        return size / sizeof(struct kinfo_proc);
      }
      return 0;

    #elif defined(__linux__)
      size_t count = 0;
      DIR* dir = opendir("/proc");
      if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
          if (entry->d_type == DT_DIR) {
            char* endptr;
            strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0') {
              ++count;
            }
          }
        }
        closedir(dir);
      }
      return count;

    #else
      return 0;
      
    #endif
  }
}
