#include "ScreenHandler.h"

namespace Project::Handlers {
  ScreenHandler::ScreenHandler(
      LogsManager& logsManager, FramesCounter& framesCounter, ConfigReader& configReader, 
      ComponentsFactory& componentsFactory, 
      GameStateManager& gameStateManager, 
      CursorHandler& cursorHandler, 
      FontHandler& fontHandler, 
      KeyHandler& keyHandler, 
      MouseHandler& mouseHandler, 
      ResourcesHandler& resourcesHandler)
      : window(nullptr), renderer(nullptr), running(false), 
      logsManager(logsManager), framesCounter(framesCounter), configReader(configReader), 
      componentsFactory(componentsFactory), gameStateManager(gameStateManager),
      cursorHandler(cursorHandler), 
      fontHandler(fontHandler), 
      keyHandler(keyHandler), 
      mouseHandler(mouseHandler), 
      resourcesHandler(resourcesHandler)
      {}

  ScreenHandler::~ScreenHandler() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
  }

  bool ScreenHandler::init() {
    if (logsManager.checkAndLogError(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0, "SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return false;
    }
    logsManager.logMessage("SDL Initialized successfully.");

    std::string windowTitle = configReader.getValue("Window", "title", "Project Doeville X");
    int screenWidth = configReader.getIntValue("Window", "width", 800);
    int screenHeight = configReader.getIntValue("Window", "height", 600);
    bool isFullscreen = configReader.getBoolValue("Window", "fullscreen", false);

    cursorWidth = configReader.getIntValue("Cursor", "width", 32);
    cursorHeight = configReader.getIntValue("Cursor", "height", 32);

    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (isFullscreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, windowFlags);
    
    if (logsManager.checkAndLogError(!window, "Window could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return false;
    }

    std::string mode = isFullscreen ? "Fullscreen" : "Windowed";
    logsManager.logMessage("Window created successfully. Mode: " + mode + ", Size: " + std::to_string(screenWidth) + "x" + std::to_string(screenHeight));
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (logsManager.checkAndLogError(!renderer,  "Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return false;
    } 
    logsManager.logMessage("Renderer created successfully.");
    
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

  void ScreenHandler::clear() {
    std::lock_guard<std::mutex> lock(renderMutex); 

    if (renderer) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
    } else {
      logsManager.logError("Renderer is null.");
    }
  }

  void ScreenHandler::render() {
    std::lock_guard<std::mutex> lock(renderMutex);
    if(logsManager.checkAndLogError(!renderer, "Renderer is null.")) {
      logsManager.flushLogs();
      return;
    }

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
    return renderer;
  }
}
