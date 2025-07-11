#include "GameEngine.h"

#include "helpers/null_checker/NullChecker.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Core {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::FramesCounter;
  using Project::Core::SDLManager;
  using Project::Handlers::ResourcesHandler;
  using Project::Factories::ComponentsFactory;
  using Project::States::GameStateManager;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::FontHandler;
  using Project::Handlers::KeyHandler;
  using Project::Handlers::MouseHandler;
  using Project::Handlers::ScreenHandler;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  GameEngine::GameEngine() :
  isRunning(false), maxFPS(Constants::DEFAULT_MAX_FPS), framesCounter(), logsManager(), configReader(logsManager), sdlManager(logsManager),
  resourcesHandler(std::make_unique<ResourcesHandler>(logsManager)),
  componentsFactory(std::make_unique<ComponentsFactory>(configReader, logsManager, *resourcesHandler)),
  gameStateManager(std::make_unique<GameStateManager>(Constants::DEFAULT_STATE_CACHE_LIMIT, logsManager)),
  cursorHandler(std::make_unique<CursorHandler>(logsManager)),
  fontHandler(std::make_unique<FontHandler>(logsManager)),
  keyHandler(std::make_unique<KeyHandler>(logsManager, sdlManager)),
  mouseHandler(std::make_unique<MouseHandler>(logsManager)),
  screenHandler(std::make_unique<ScreenHandler>(
    logsManager, framesCounter, configReader, sdlManager,
    *componentsFactory, *gameStateManager,
    *cursorHandler, *fontHandler, *keyHandler,
    *mouseHandler, *resourcesHandler))
  {
    cleanupHandlers.emplace_back(&sdlManager, "SDLManager is null.");
    cleanupHandlers.emplace_back(gameStateManager.get(), "GameStateManager is null.");
    cleanupHandlers.emplace_back(cursorHandler.get(), "CursorHandler is null.");
    cleanupHandlers.emplace_back(fontHandler.get(), "FontHandler is null.");
    cleanupHandlers.emplace_back(resourcesHandler.get(), "ResourcesHandler is null.");
  }
  
  GameEngine::~GameEngine() {
    if (isRunning) {
      clean();
    }
  }

  void GameEngine::init() {
    if (logsManager.checkAndLogError(!configReader.loadConfig(Keys::CONFIG_FILE), "Failed to load config.ini")) {
      logsManager.flushLogs();
      return;
    }

    std::string title = configReader.getValue(Keys::WINDOW_SECTION, Keys::WINDOW_TITLE, Constants::PROJECT_NAME);
    int screenWidth = configReader.getIntValue(Keys::WINDOW_SECTION, Keys::WINDOW_WIDTH, Constants::DEFAULT_SCREEN_WIDTH);
    int screenHeight = configReader.getIntValue(Keys::WINDOW_SECTION, Keys::WINDOW_HEIGHT, Constants::DEFAULT_SCREEN_HEIGHT);
    bool isFullscreen = configReader.getBoolValue(Keys::WINDOW_SECTION, Keys::WINDOW_FULLSCREEN, false);
    bool vsync = configReader.getBoolValue(Keys::WINDOW_SECTION, Keys::WINDOW_VSYNC, true);

    if (!sdlManager.init(title, screenWidth, screenHeight, isFullscreen, vsync)) {
      logsManager.logError("Failed to initialize SDLManager.");
      return;
    }

    SDL_ShowCursor(SDL_DISABLE);
    std::string fontRelPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    if (!Project::Helpers::checkNotNull(logsManager, resourcesHandler.get(), "ResourcesHandler is null.")) {
      return;
    }
    std::string fontPath = resourcesHandler->getResourcePath(fontRelPath);

    if (!Project::Helpers::checkNotNull(logsManager, screenHandler.get(), "ScreenHandler is null.")) {
      return;
    }
    if (logsManager.checkAndLogError(!screenHandler->init(), "Screen Handler initialization failed!")) {
      isRunning = false;
      logsManager.flushLogs();
      return;
    }

    if (!Project::Helpers::checkNotNull(logsManager, fontHandler.get(), "FontHandler is null.")) {
      return;
    }

    if (logsManager.checkAndLogError(!fontHandler->loadFont(Constants::DEFAULT_FONT, fontPath.c_str(), Constants::DEFAULT_FONT_SIZE), "Failed to load required font 'system'!")) {
      logsManager.flushLogs();
      return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
      logsManager.logError("Failed to initialize SDL_image for PNG: " + std::string(IMG_GetError()));
      return;
    }

    if (Project::Helpers::checkNotNull(logsManager, componentsFactory.get(), "ComponentsFactory is null.")) {
      componentsFactory->setRenderer(screenHandler->getRenderer());
    } else {
      return;
    }

    if (Project::Helpers::checkNotNull(logsManager, keyHandler.get(), "KeyHandler is null.")) {
      keyHandler->setKeyBinding(Project::Handlers::KeyAction::HELP_TOGGLE, Constants::KEY_FUNC_HELP);
    } else {
      return;
    }

    logsManager.logMessage("Game Engine has been initialized successfully.");
    logsManager.flushLogs();
    isRunning = true;
  }

  void GameEngine::run() {
    double accumulator = 0.0;
    const double fixedDelta = 1.0 / Constants::TARGET_FPS;
    
    while (isRunning) {
      Uint64 frameStartTime = SDL_GetPerformanceCounter();

      framesCounter.update();
      accumulator += framesCounter.getDeltaTime();

      handleEvents();
      if (!isRunning) {
        break;
      }
  
      while (accumulator >= fixedDelta) {
        update(static_cast<float>(fixedDelta));
        accumulator -= fixedDelta;
      }

      render();
      handleFrameRate(frameStartTime);
    }
  }

  void GameEngine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      if (Project::Helpers::checkNotNull(logsManager, keyHandler.get(), "KeyHandler is null.")) {
        keyHandler->handleInput(event);
      }
      if (Project::Helpers::checkNotNull(logsManager, mouseHandler.get(), "MouseHandler is null.")) {
        mouseHandler->handleEvent(event);
      }

      if (event.type == SDL_QUIT) {
        logsManager.logMessage("Quit event received");
        clean();
      }
    }

    if (sdlManager.isExitRequested()) {
      logsManager.logMessage("Exit flag detected");
      clean();
      sdlManager.clearExitRequest();
    }
  }

  void GameEngine::update(float deltaTime) {
    if (Project::Helpers::checkNotNull(logsManager, keyHandler.get(), "KeyHandler is null.")) {
      if (keyHandler->isGameFrozen()) {
        return;
      }
    } else {
      return;
    }

    std::lock_guard<std::mutex> lock(updateMutex);
    if (Project::Helpers::checkNotNull(logsManager, gameStateManager.get(), "GameStateManager is null.")) {
      gameStateManager->update(deltaTime);
    } else {
      return;
    }

    sdlManager.clear();
  }

  void GameEngine::render() {
    if (Project::Helpers::checkNotNull(logsManager, keyHandler.get(), "KeyHandler is null.")) {
      if (keyHandler->isGameFrozen()) {
        return;
      }
    } else {
      return;
    }

    if (Project::Helpers::checkNotNull(logsManager, screenHandler.get(), "ScreenHandler is null.")) {
      screenHandler->render();
    }
  }

  void GameEngine::handleFrameRate(Uint64 frameStartTime) {
    const double BASE_VALUE = 1.0;
    const double targetFrameDuration = BASE_VALUE / maxFPS;

    Uint64 frameEndTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    double frameDuration = (frameEndTime - frameStartTime) / static_cast<double>(frequency);

    if (frameDuration < targetFrameDuration) {
      Uint32 delayMs = static_cast<Uint32>((targetFrameDuration - frameDuration) * Constants::MILLISECONDS_PER_SECOND);
      if (delayMs > 0) {
        SDL_Delay(delayMs);
      }
    }
  }

  void GameEngine::clean() {
    logsManager.logMessage("Cleaning up game engine...");

    for (const auto& item : cleanupHandlers) {
      if (Project::Helpers::checkNotNull(logsManager, item.first, item.second)) {
        item.first->cleanup();
      }
    }
    
    IMG_Quit();
    isRunning = false;

    logsManager.logMessage("Game engine cleanup complete.");
    logsManager.flushLogs();
  }
}