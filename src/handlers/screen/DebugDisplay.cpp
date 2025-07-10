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
    std::string fpsText = "FPS: " + std::to_string(fps);
    SDL_Texture* fpsTexture = fontHandler.renderText(renderer, fpsText, "system", debugTextColor);

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
    double frameTime = framesCounter.getDeltaTime() * Constants::MILLISECONDS_PER_SECOND;
    std::string benchmarkText = "Frame: " + std::to_string(static_cast<int>(frameTime)) + " ms";
    SDL_Texture* benchmarkTexture = fontHandler.renderText(renderer, benchmarkText, "system", debugTextColor);

    if (benchmarkTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(benchmarkTexture, nullptr, nullptr, &textWidth, &textHeight);

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
      int yPosition = Constants::DEBUG_TEXT_MARGIN + fpsTextHeight + Constants::DEBUG_TEXT_HEIGHT_OFFSET;
      SDL_Rect destRect = {screenWidth - textWidth - Constants::DEBUG_TEXT_MARGIN, yPosition, textWidth, textHeight};

      SDL_RenderCopy(renderer, benchmarkTexture, nullptr, &destRect);
      SDL_DestroyTexture(benchmarkTexture);
    } else {
      logsManager.logError("Failed to render benchmark text.");
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

    #elif defined(__APPLE__) && defined(__MACH__)

    #elif defined(__linux__)

    #else
    return 0;
    
    #endif
  }
}
