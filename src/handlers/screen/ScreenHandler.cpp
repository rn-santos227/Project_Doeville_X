#include "ScreenHandler.h"

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Utilities::FramesCounter;
  using Project::Utilities::ConfigReader;
  using Project::Core::SDLManager;
  using Project::Factories::ComponentsFactory;
  using Project::States::GameStateManager;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::FontHandler;
  using Project::Handlers::KeyHandler;
  using Project::Handlers::MouseHandler;
  using Project::Handlers::ResourcesHandler;
  using Project::Services::ScriptingService;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

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
      resourcesHandler(resourcesHandler),
      debugDisplay(logsManager, framesCounter, configReader, sdlManager, fontHandler, mouseHandler)
    {}

  ScreenHandler::~ScreenHandler() = default;

  bool ScreenHandler::init() {
    SDL_Renderer* renderer = sdlManager.getRenderer();
    componentsFactory.setRenderer(renderer);
    componentsFactory.setKeyHandler(&keyHandler);

    int screenWidth = 0;
    int screenHeight = 0;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    cameraHandler.setSize(screenWidth, screenHeight);

    cursorWidth = configReader.getIntValue(Keys::CURSOR_SECTION, Keys::CURSOR_W, Constants::DEFAULT_CURSOR_SIZE);
    cursorHeight = configReader.getIntValue(Keys::CURSOR_SECTION, Keys::CURSOR_H, Constants::DEFAULT_CURSOR_SIZE);

    cursorHandler.setRenderer(renderer);
    std::string cursorPath = resourcesHandler.getResourcePath(Constants::DEFAULT_CURSOR_PATH);
    cursorHandler.loadCursor(CursorState::DEFAULT, cursorPath.c_str());
    cursorHandler.setCursorState(CursorState::DEFAULT);

    scriptingService = std::make_unique<ScriptingService>(
      renderer, logsManager, resourcesHandler, componentsFactory, gameStateManager
    );

    if (logsManager.checkAndLogError(!scriptingService, "Failed to validate main.lua script.")) {
      return false;
    }

    std::string scriptPath = configReader.getValue(Keys::PATHS_SECTION, Keys::PATH_SCRIPTS, Constants::DEFAULT_SCRIPT_PATH);
    scriptingService->loadScriptsFromFolder(scriptPath);

    std::string initialState = configReader.getValue(Keys::GAME_SECTION, Keys::GAME_INITIAL_STATE, Constants::DEFAULT_INITIAL_STATE);
    gameStateManager.setInitialState(initialState);

    debugDisplay.init();
    running = true;
    return true;
  }

  void ScreenHandler::render() {
    std::lock_guard<std::mutex> lock(renderMutex);
    SDL_Renderer* renderer = sdlManager.getRenderer();

    gameStateManager.render();
    if (keyHandler.isGameDebugMode()) {
      debugDisplay.render();
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

    sdlManager.present(); 
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
  
  SDL_Renderer* ScreenHandler::getRenderer() const {
    return sdlManager.getRenderer();
  }

  SDL_Window* ScreenHandler::getWindow() const {
    return sdlManager.getWindow();
  }
}
