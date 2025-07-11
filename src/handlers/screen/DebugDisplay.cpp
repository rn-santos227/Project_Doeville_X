#include "DebugDisplay.h"

#include <cstdio>
#include <iomanip>
#include <sstream>

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

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Utilities::FramesCounter;
  using Project::Utilities::ConfigReader;
  using Project::Core::SDLManager;
  using Project::Handlers::FontHandler;
  using Project::Handlers::MouseHandler;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  DebugDisplay::DebugDisplay(LogsManager& logsManager,
    FramesCounter& framesCounter, ConfigReader& configReader,
    SDLManager& sdlManager, FontHandler& fontHandler, MouseHandler& mouseHandler)
    : logsManager(logsManager), framesCounter(framesCounter), configReader(configReader),
      sdlManager(sdlManager), fontHandler(fontHandler), mouseHandler(mouseHandler) {
    debugTextColor = configReader.getColorValue(Keys::DEBUG_SECTION, Keys::DEBUG_TEXT_COLOR, debugTextColor);
    axisXColor = configReader.getColorValue(Keys::AXIS_SECTION, Keys::AXIS_X_COLOR, axisXColor);
    axisYColor = configReader.getColorValue(Keys::AXIS_SECTION, Keys::AXIS_Y_COLOR, axisYColor);
  }

  void DebugDisplay::render() {
    renderGrid();
    renderAxes();

    SDL_Renderer* renderer = sdlManager.getRenderer();
    int yOffset = Constants::DEBUG_TEXT_MARGIN;

    auto formatLine = [](const std::string& prefix, auto value, const std::string& suffix = "") {
      std::ostringstream oss;
      oss << std::left << std::setw(Constants::DEBUG_PREFIX_WIDTH) << prefix
          << std::right << std::setw(Constants::DEBUG_LINE_SPACING) << value
          << suffix;
      return oss.str();
    };

    const std::vector<std::string> debugLines = {
      formatLine(Constants::DEBUG_FPS_PREFIX, framesCounter.getFPS()),
      formatLine(Constants::DEBUG_FRAME_PREFIX, static_cast<int>(framesCounter.getDeltaTime() * Constants::MILLISECONDS_PER_SECOND), Constants::DEBUG_FRAME_SUFFIX),
      formatLine(Constants::DEBUG_MEM_PREFIX, getProcessMemoryUsageMB(), Constants::DEBUG_MEM_SUFFIX),
      formatLine(Constants::DEBUG_PROC_PREFIX, getProcessCount())
    };

    for (const std::string& text : debugLines) {
      SDL_Texture* texture = fontHandler.renderText(renderer, text, Constants::DEFAULT_FONT, debugTextColor);
      if (texture) {
        int textWidth, textHeight;
        SDL_QueryTexture(texture, nullptr, nullptr, &textWidth, &textHeight);

        int screenWidth, screenHeight;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

        SDL_Rect destRect = {
          screenWidth - textWidth - Constants::DEBUG_TEXT_MARGIN,
          yOffset,
          textWidth,
          textHeight
        };

        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        SDL_DestroyTexture(texture);

        yOffset += textHeight + Constants::DEBUG_LINE_SPACING;
      } else {
        logsManager.logError("Failed to render debug text: " + text);
      }
    }

    renderMousePosition();
  }

  void DebugDisplay::renderAxes() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    SDL_SetRenderDrawColor(renderer, axisXColor.r, axisXColor.g, axisXColor.b, axisXColor.a);
    SDL_RenderDrawLine(renderer, 0, screenHeight / 2, screenWidth, screenHeight / 2);

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

    std::string mouseText = "Mouse: (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";
    SDL_Texture* texture = fontHandler.renderText(renderer, mouseText, "system", debugTextColor);

    if (texture) {
      int textWidth, textHeight;
      SDL_QueryTexture(texture, nullptr, nullptr, &textWidth, &textHeight);

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {
        Constants::DEBUG_TEXT_MARGIN,
        screenHeight - textHeight - Constants::DEBUG_TEXT_MARGIN,
        textWidth,
        textHeight
      };

      SDL_RenderCopy(renderer, texture, nullptr, &destRect);
      SDL_DestroyTexture(texture);
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
    long rss = 0;
    FILE* fp = fopen("/proc/self/statm", "r");
    if (fp) {
      if (fscanf(fp, "%*s%ld", &rss) != 1) rss = 0L;
      fclose(fp);
    }
    long pageSize = sysconf(_SC_PAGESIZE);
    return static_cast<size_t>(rss * pageSize) / Constants::BYTES_PER_MEGABYTE;
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
          if (*endptr == '\0') ++count;
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
