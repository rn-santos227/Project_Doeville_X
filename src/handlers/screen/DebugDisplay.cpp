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

    if( benchmarkTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(benchmarkTexture, nullptr, nullptr, &textWidth, &textHeight);

      int screenWidth, screenHeight;
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
}
