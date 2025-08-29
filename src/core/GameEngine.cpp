#include "GameEngine.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include "helpers/null_checker/NullChecker.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "platform/renderer/OpenGLRenderer.h"
#include "platform/renderer/VulkanRenderer.h"
#include "utilities/exception/EngineException.h"
#include "utilities/profiler/Profiler.h"
#include "utilities/thread/ThreadPool.h"

namespace Project::Core {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::EngineException;
  using Project::Utilities::ThreadPool;
  using Project::Factories::ComponentsFactory;
  using Project::Handlers::ResourcesHandler;
  using Project::Platform::Platform;
  using Project::Platform::SDLPlatform;
  using Project::States::GameStateManager;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::FontHandler;
  using Project::Handlers::KeyHandler;
  using Project::Handlers::MouseHandler;
  using Project::Handlers::ScreenHandler;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  GameEngine::GameEngine() :
  isRunning(false), maxFPS(Constants::DEFAULT_MAX_FPS), entityLoadFactor(Constants::DEFAULT_WHOLE), frameTimeAvg(0.0),
  framesCounter(), logsManager(), configReader(logsManager),
  platform(std::make_unique<SDLPlatform>(logsManager)),
  resourcesHandler(std::make_unique<ResourcesHandler>(logsManager)),
  componentsFactory(std::make_unique<ComponentsFactory>(logsManager, configReader, *resourcesHandler)),
  sceneCache(std::make_unique<Project::Services::SceneCacheService>(logsManager)),
  gameStateManager(std::make_unique<GameStateManager>(Constants::DEFAULT_STATE_CACHE_LIMIT, logsManager, platform.get(), nullptr, sceneCache.get())),
  cursorHandler(std::make_unique<CursorHandler>(logsManager)),
  fontHandler(std::make_unique<FontHandler>(logsManager)),
  keyHandler(std::make_unique<KeyHandler>(logsManager, *platform, gameStateManager.get())),
  mouseHandler(std::make_unique<MouseHandler>(logsManager)),
  screenHandler(std::make_unique<ScreenHandler>(
    logsManager, framesCounter, configReader, *platform,
    *componentsFactory, *gameStateManager,
    *cursorHandler, *fontHandler, *keyHandler,
    *mouseHandler, *resourcesHandler))
  {
    if (gameStateManager && cursorHandler) {
      gameStateManager->setCursorHandler(cursorHandler.get());
    }
    cleanupHandlers.emplace_back(platform.get(), "Platform is null.");
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
    try {
      if (logsManager.checkAndLogError(!configReader.loadConfig(Keys::CONFIG_FILE), "Failed to load config.ini")) {
        throw EngineException("Failed to load configuration", Project::Utilities::ErrorCategory::CONFIG);
      }

      if (componentsFactory) {
        componentsFactory->configurePools();
      }

      std::string title = configReader.getValue(Keys::WINDOW_SECTION, Keys::WINDOW_TITLE, Constants::PROJECT_NAME);
      int screenWidth = configReader.getIntValue(Keys::WINDOW_SECTION, Keys::WINDOW_WIDTH, Constants::DEFAULT_SCREEN_WIDTH);
      int screenHeight = configReader.getIntValue(Keys::WINDOW_SECTION, Keys::WINDOW_HEIGHT, Constants::DEFAULT_SCREEN_HEIGHT);
      bool isFullscreen = configReader.getBoolValue(Keys::WINDOW_SECTION, Keys::WINDOW_FULLSCREEN, false);
      
      bool opengl = configReader.getBoolValue(Keys::VIDEO_SECTION, Keys::VIDEO_OPENGL, true);
      bool vsync = configReader.getBoolValue(Keys::VIDEO_SECTION, Keys::VIDEO_VSYNC, true);

      // if (opengl) {
      //   platform->setRendererAPI(std::make_unique<Project::Platform::OpenGLRenderer>());
      // } else {
      //   platform->setRendererAPI(std::make_unique<Project::Platform::VulkanRenderer>());
      // }

      if (!platform->init(title, screenWidth, screenHeight, isFullscreen, vsync, opengl)) {
        logsManager.logError("Failed to initialize SDL platform.");
        throw EngineException("SDL initialization failed", Project::Utilities::ErrorCategory::SDL);
      }

      SDL_ShowCursor(SDL_DISABLE);
      std::string fontRelPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
      if (!Project::Helpers::checkNotNull(logsManager, resourcesHandler.get(), "ResourcesHandler is null.")) {
        throw EngineException("ResourcesHandler is null", Project::Utilities::ErrorCategory::RESOURCE);
      }
      std::string fontPath = resourcesHandler->getResourcePath(fontRelPath);

      if (!Project::Helpers::checkNotNull(logsManager, screenHandler.get(), "ScreenHandler is null.")) {
        throw EngineException("ScreenHandler is null", Project::Utilities::ErrorCategory::SDL);
      }

      if (logsManager.checkAndLogError(!screenHandler->init(), "Screen Handler initialization failed!")) {
        isRunning = false;
        logsManager.flushLogs();
        throw EngineException("Screen handler initialization failed", Project::Utilities::ErrorCategory::SDL);
      }

      if (!Project::Helpers::checkNotNull(logsManager, fontHandler.get(), "FontHandler is null.")) {
        throw EngineException("FontHandler is null", Project::Utilities::ErrorCategory::RESOURCE);
      }

      if (logsManager.checkAndLogError(!fontHandler->loadFont(Constants::DEFAULT_FONT, fontPath.c_str(), Constants::DEFAULT_FONT_SIZE), "Failed to load required font 'system'!")) {
        logsManager.flushLogs();
        throw EngineException("Font load failed", Project::Utilities::ErrorCategory::RESOURCE);
      }

      if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        logsManager.logError("Failed to initialize SDL_image for PNG: " + std::string(IMG_GetError()));
        throw EngineException("SDL_image initialization failed", Project::Utilities::ErrorCategory::SDL);
      }

      if (Project::Helpers::checkNotNull(logsManager, componentsFactory.get(), "ComponentsFactory is null.")) {
        componentsFactory->setRenderer(screenHandler->getRenderer());
      } else {
        throw EngineException("ComponentsFactory is null", Project::Utilities::ErrorCategory::RESOURCE);
      }

      if (Project::Helpers::checkNotNull(logsManager, keyHandler.get(), "KeyHandler is null.")) {
        keyHandler->setKeyBinding(Project::Handlers::KeyAction::HELP_TOGGLE, Constants::KEY_FUNC_HELP);
      } else {
        throw EngineException("KeyHandler is null", Project::Utilities::ErrorCategory::INPUT);
      }

      auto& threadPool = Project::Utilities::ThreadPool::getInstance();
      threadPool.setLogger(&logsManager);
      logsManager.logMessage("Game Engine has been initialized successfully.");
      logsManager.flushLogs();
      isRunning = true;
    } catch (const Project::Utilities::EngineException& e) {
      logsManager.logError(e.what());
      logsManager.flushLogs();
      isRunning = false;
    } catch (const std::exception& e) {
      logsManager.logError(std::string("Unhandled exception: ") + e.what());
      logsManager.flushLogs();
      isRunning = false;
    }
  }

  void GameEngine::run() {
    double accumulator = 0.0;
    const double fixedDelta = Constants::DEFAULT_WHOLE / Constants::TARGET_FPS;
    try {
      while (isRunning) {
        Project::Utilities::Profiler::getInstance().beginFrame();
        Uint64 frameStartTime = SDL_GetPerformanceCounter();

        framesCounter.update();
        Project::Utilities::Profiler::getInstance().addTime(
          Constants::CPU_FRAME,
          framesCounter.getDeltaTime() * Project::Libraries::Constants::MILLISECONDS_PER_SECOND
        );
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
    } catch (const std::exception& e) {
      logsManager.logError(std::string("Runtime exception: ") + e.what());
      logsManager.flushLogs();
      clean();
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

    if (Project::Helpers::checkNotNull(logsManager, mouseHandler.get(), "MouseHandler is null.")) {
      mouseHandler->updateMousePosition();
    }

    if (platform->isExitRequested()) {
      logsManager.logMessage("Exit flag detected");
      clean();
      platform->clearExitRequest();
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

    if (Project::Helpers::checkNotNull(logsManager, gameStateManager.get(), "GameStateManager is null.")) {
      gameStateManager->update(deltaTime);
    } else {
      return;
    }

    platform->clear();
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
    Uint64 frameEndTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    double frameDuration = (frameEndTime - frameStartTime) / static_cast<double>(frequency);

    frameTimeAvg = frameTimeAvg * 0.9 + frameDuration * 0.1;

    double idealFrameDuration = Constants::DEFAULT_WHOLE / maxFPS;
    double perfFactor = std::max(static_cast<double>(Constants::DEFAULT_WHOLE), frameTimeAvg / idealFrameDuration);

    double entityFactor = Constants::DEFAULT_WHOLE;
    if (gameStateManager) {
      size_t entityCount = gameStateManager->getActiveEntityCount();
      entityFactor = static_cast<double>(entityCount) / Constants::ENTITY_OPTIMIZATION_THRESHOLD;
      entityFactor = std::max(entityFactor, static_cast<double>(Constants::DEFAULT_WHOLE));
    }

    double targetFactor = std::max(perfFactor, entityFactor);
    entityLoadFactor = entityLoadFactor * 0.9 + targetFactor * 0.1;

    double currentMaxFPS = maxFPS / entityLoadFactor;
    if (currentMaxFPS < Constants::TARGET_FPS) {
      currentMaxFPS = Constants::TARGET_FPS;
    }

    const double targetFrameDuration = Constants::DEFAULT_WHOLE / currentMaxFPS;

    if (frameDuration < targetFrameDuration) {
      double remaining = targetFrameDuration - frameDuration;
      Uint32 delayMs = static_cast<Uint32>(remaining * Constants::MILLISECONDS_PER_SECOND);

      if (delayMs > 0) {
        SDL_Delay(delayMs);
        remaining -= static_cast<double>(delayMs) / Constants::MILLISECONDS_PER_SECOND;
      }

      if (remaining > 0.0) {
        std::this_thread::sleep_for(std::chrono::duration<double>(remaining));
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

    if (sceneCache) {
      sceneCache->logDiagnostics();
    }
    logsManager.logMessage("Game engine cleanup complete.");
    logsManager.flushLogs();
  }
}
