#include "ScreenHandler.h"

namespace Project::Handlers {
  ScreenHandler::ScreenHandler(
      LogsManager& logsManager, FramesCounter& framesCounter, ConfigReader& configReader, 
      SDLManager& sdlManager,
      ComponentsFactory& componentsFactory, 
      GameStateManager& gameStateManager, 
      CursorHandler& cursorHandler, 
      FontHandler& fontHandler, 
      KeyHandler& keyHandler, 
      MouseHandler& mouseHandler, 
      ResourcesHandler& resourcesHandler)
      : running(false), 
      logsManager(logsManager), framesCounter(framesCounter), 
      configReader(configReader), sdlManager(sdlManager), 
      componentsFactory(componentsFactory), gameStateManager(gameStateManager),
      cursorHandler(cursorHandler), 
      fontHandler(fontHandler), 
      keyHandler(keyHandler), 
      mouseHandler(mouseHandler), 
      resourcesHandler(resourcesHandler)
      {}

  ScreenHandler::~ScreenHandler() = default;

  bool ScreenHandler::init() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    cursorWidth = configReader.getIntValue("Cursor", "width", 32);
    cursorHeight = configReader.getIntValue("Cursor", "height", 32);
    
    cursorHandler.setRenderer(renderer);
    std::string cursorPath = resourcesHandler.getResourcePath("resources/system/cursor_default.png");
    cursorHandler.loadCursor(CursorState::DEFAULT, cursorPath.c_str());
    cursorHandler.setCursorState(CursorState::DEFAULT);

    scriptingService = std::make_unique<ScriptingService>(
      renderer, logsManager, componentsFactory, gameStateManager
    );

    if(logsManager.checkAndLogError(!scriptingService, "Failed to validate main.lua script.")) {
      logsManager.flushLogs();
      return false;
    }

    std::string scriptPath = configReader.getValue("Paths", "scripts", "scripts/");
    scriptingService->loadScriptsFromFolder(scriptPath);

    std::string initialState = configReader.getValue("Game", "initial_state", "MainMenu");
    gameStateManager.setInitialState(initialState);

    running = true;
    return true;
  }

  void ScreenHandler::render() {
    std::lock_guard<std::mutex> lock(renderMutex);
    SDL_Renderer* renderer = sdlManager.getRenderer();

    gameStateManager.render();
    if (keyHandler.isGameDebugMode()) {
      renderFPS();
      renderMousePosition();
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    mouseX = std::max(0, std::min(mouseX, screenWidth - cursorWidth));
    mouseY = std::max(0, std::min(mouseY, screenHeight - cursorHeight));

    SDL_Texture* texture = cursorHandler.getCursorTexture(CursorState::DEFAULT);
    if (texture) {
      SDL_Rect dstRect = { mouseX, mouseY, cursorWidth, cursorHeight };
      SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    } else {
      logsManager.logError("Failed to render cursor: Texture is null.");
    }

    SDL_RenderPresent(renderer);
  }

  void ScreenHandler::handleEvents() {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
      keyHandler.handleInput(e);

      if (e.type == SDL_MOUSEMOTION) {
        int x = e.motion.x;
        int y = e.motion.y;
      }
    }
  }

  bool ScreenHandler::isRunning() const {
    return running;
  }

  void ScreenHandler::renderFPS() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    int fps = framesCounter.getFPS();
    std::string fpsText = "FPS: " + std::to_string(fps);
    
    SDL_Color color = {144, 238, 144, 255};
    SDL_Texture* fpsTexture = fontHandler.renderText(renderer, fpsText, "system", color);
    
    if (fpsTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(fpsTexture, nullptr, nullptr, &textWidth, &textHeight);

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {screenWidth - textWidth - 10, 10, textWidth, textHeight};
      SDL_RenderCopy(renderer, fpsTexture, nullptr, &destRect);
      SDL_DestroyTexture(fpsTexture);
    } else {
      logsManager.logError("Failed to render FPS text.");
    }
  }

  void ScreenHandler::renderMousePosition() {
    mouseHandler.updateMousePosition();
    int mouseX = mouseHandler.getMouseX();
    int mouseY = mouseHandler.getMouseY();

    std::string mousePositionText = "Mouse: (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";
    SDL_Color color = {144, 238, 144, 255};
    SDL_Texture* mouseTexture  = fontHandler.renderText(renderer, mousePositionText, "system", color);

    if (mouseTexture) {
      int textWidth, textHeight;
      SDL_QueryTexture(mouseTexture, nullptr, nullptr, &textWidth, &textHeight);

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      SDL_Rect destRect = {10, screenHeight - textHeight - 10, textWidth, textHeight};
      SDL_RenderCopy(renderer, mouseTexture, nullptr, &destRect);
      SDL_DestroyTexture(mouseTexture);
    } else {
      logsManager.logError("Failed to render mouse position.");
    }
  }
  
  SDL_Renderer* ScreenHandler::getRenderer() const {
    return sdlManager.getRenderer();
  }

  SDL_Window* ScreenHandler::getWindow() const {
    return sdlManager.getWindow();
}
