#include "DebugDisplay.h"

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
    renderAxes();
    renderFPS();
    renderBenchmark();
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

    } else {
      logsManager.logError("Failed to render FPS text.");
    }
  }
}
