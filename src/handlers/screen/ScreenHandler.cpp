#include "ScreenHandler.h"

namespace Project::Handlers {
  ScreenHandler::ScreenHandler(GameStateManager& gameStateManager, ConfigHandler& configHandler, CursorHandler& cursorHandler, FontHandler& fontHandler, KeyHandler& keyHandler, MouseHandler& mouseHandler, ResourcesHandler& resourcesHandler, LogsManager& logsManager, FramesCounter& framesCounter)
      : window(nullptr), renderer(nullptr), running(false), 
      cursorHandler(cursorHandler), fontHandler(fontHandler), keyHandler(keyHandler), mouseHandler(mouseHandler), resourcesHandler(resourcesHandler),
      gameStateManager(gameStateManager),
      logsManager(logsManager), framesCounter(framesCounter) {}

  ScreenHandler::~ScreenHandler() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }

  bool ScreenHandler::init() {
    if (logsManager.checkAndLogError(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0, "SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return false;
    }
    logsManager.logMessage("SDL Initialized successfully.");

    window = SDL_CreateWindow("Project Doeville X", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    
    if (logsManager.checkAndLogError(!window, "Window could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return false;
    }
    logsManager.logMessage("Window created successfully.");
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (logsManager.checkAndLogError(!renderer,  "Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return false;
    } 
    logsManager.logMessage("Renderer created successfully.");
    
    cursorHandler.setRenderer(renderer);
    std::string cursorPath = ResourcesHandler::getResourcePath("resources/system/cursor_default.png");
    cursorHandler.loadCursor(CursorState::DEFAULT, cursorPath.c_str());
    cursorHandler.setCursorState(CursorState::DEFAULT);

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

    if (renderer) {
      if (keyHandler.isGameDebugMode()) {
        renderFPS();
        renderMousePosition();
      }

      gameStateManager.render();

      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);

      int screenWidth, screenHeight;
      SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

      mouseX = std::max(0, std::min(mouseX, screenWidth - 32));
      mouseY = std::max(0, std::min(mouseY, screenHeight - 32));

      SDL_Texture* texture = cursorHandler.getCursorTexture(CursorState::DEFAULT);
      if (texture) {
        SDL_Rect dstRect = { mouseX, mouseY, 32, 32 };
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
      } else {
        logsManager.logError("Failed to render cursor: Texture is null.");
      }

    } else {
      logsManager.logError("Renderer is null.");
    }
  }

  void ScreenHandler::update() {
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

      SDL_Rect destRect = {800 - textWidth - 10, 10, textWidth, textHeight};
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
