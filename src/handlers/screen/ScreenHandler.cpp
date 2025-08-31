#include "ScreenHandler.h"

#include <algorithm>

#include "components/graphics_component/GraphicsComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/vision_component/VisionComponent.h"
#include "handlers/camera/CameraTypeResolver.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Utilities::FramesCounter;
  using Project::Utilities::ConfigReader;
  using Project::Factories::ComponentsFactory;
  using Project::States::GameStateManager;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::FontHandler;
  using Project::Handlers::KeyHandler;
  using Project::Handlers::MouseHandler;
  using Project::Handlers::ResourcesHandler;
  using Project::Platform::Platform;
  using Project::Services::ScriptingService;
  using Project::Services::StyleService;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  ScreenHandler::ScreenHandler(
      LogsManager& logsManager, FramesCounter& framesCounter, ConfigReader& configReader, 
      Platform& platform,
      ComponentsFactory& componentsFactory, 
      GameStateManager& gameStateManager, 
      CursorHandler& cursorHandler, 
      FontHandler& fontHandler, 
      KeyHandler& keyHandler, 
      MouseHandler& mouseHandler,
      ResourcesHandler& resourcesHandler)
      : running(false), 
      logsManager(logsManager), framesCounter(framesCounter),
      configReader(configReader), platform(platform),
      componentsFactory(componentsFactory), gameStateManager(gameStateManager),
      cursorHandler(cursorHandler),
      fontHandler(fontHandler),
      keyHandler(keyHandler),
      mouseHandler(mouseHandler),
      resourcesHandler(resourcesHandler),
      debugDisplay(logsManager, framesCounter, configReader, platform, fontHandler, mouseHandler)
    {}

  ScreenHandler::~ScreenHandler() = default;

  bool ScreenHandler::init() {
    SDL_Renderer* renderer = platform.getRenderer();
    componentsFactory.setRenderer(renderer);
    componentsFactory.setKeyHandler(&keyHandler);
    componentsFactory.setMouseHandler(&mouseHandler);
    componentsFactory.setCursorHandler(&cursorHandler);

    int screenWidth = 0;
    int screenHeight = 0;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    
    double scale = configReader.getDoubleValue(Keys::WINDOW_SECTION, Keys::WINDOW_SCALE_FACTORY, 1.0);
    int camW = static_cast<int>(screenWidth * scale);
    int camH = static_cast<int>(screenHeight * scale);

    cameraHandler.setSize(camW, camH);
    std::string camTypeStr = configReader.getValue(Keys::CAMERA_SECTION, Keys::CAMERA_TYPE, Project::Libraries::Categories::Cameras::FREE_ROAMING);
    cameraHandler.setCameraType(CameraTypeResolver::resolve(camTypeStr));
    cameraHandler.setCullingOffset(Constants::DEFAULT_CAMERA_CULL_OFFSET, Constants::DEFAULT_CAMERA_CULL_OFFSET);
    
    Project::Components::GraphicsComponent::setCameraHandler(&cameraHandler);
    Project::Components::BoundingBoxComponent::setCameraHandler(&cameraHandler);
    Project::Components::VisionComponent::setCameraHandler(&cameraHandler);
    
    componentsFactory.setCameraHandler(&cameraHandler);
    cursorWidth = configReader.getIntValue(Keys::CURSOR_SECTION, Keys::CURSOR_W, Constants::DEFAULT_CURSOR_SIZE);
    cursorHeight = configReader.getIntValue(Keys::CURSOR_SECTION, Keys::CURSOR_H, Constants::DEFAULT_CURSOR_SIZE);

    cursorHandler.setRenderer(renderer);
    std::string cursorPath = resourcesHandler.getResourcePath(Constants::DEFAULT_CURSOR_PATH);
    cursorHandler.loadCursor(CursorState::DEFAULT, cursorPath.c_str());
    
    std::string handPath = resourcesHandler.getResourcePath(Constants::HAND_CURSOR_PATH);
    cursorHandler.loadCursor(CursorState::HOVER, handPath.c_str());
   
    std::string textCursorPath = resourcesHandler.getResourcePath(Constants::TEXT_CURSOR_PATH);
    cursorHandler.loadCursor(CursorState::TEXT, textCursorPath.c_str());

    cursorHandler.setCursorState(CursorState::DEFAULT);
    
    styleService = std::make_unique<StyleService>(logsManager, resourcesHandler);
    scriptingService = std::make_unique<ScriptingService>(
      renderer, platform, logsManager, configReader, resourcesHandler, componentsFactory, gameStateManager
    );

    if (logsManager.checkAndLogError(!scriptingService, "Failed to validate main.lua script.")) {
      return false;
    }

    if (logsManager.checkAndLogError(!styleService, "Failed to validate style sheet.")) {
      return false;
    }

    std::string stylePath = configReader.getValue(Keys::PATHS_SECTION, Keys::PATH_STYLES, Constants::DEFAULT_STYLE_PATH);
    styleService->loadStylesFromFolder(stylePath);

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
    SDL_Renderer* renderer = platform.getRenderer();

    gameStateManager.render();
    if (keyHandler.isGameDebugMode()) {
      gameStateManager.renderDebug();
      debugDisplay.render();
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    mouseX = std::max(0, std::min(mouseX, screenWidth - cursorWidth));
    mouseY = std::max(0, std::min(mouseY, screenHeight - cursorHeight));

    SDL_Texture* texture = cursorHandler.getCursorTexture(cursorHandler.getCursorState());
    if (texture) {
      SDL_Rect dstRect = { mouseX, mouseY, cursorWidth, cursorHeight };
      SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    } else {
      logsManager.logError("Failed to render cursor: Texture is null.");
    }

    platform.present(); 
  }

  void ScreenHandler::handleEvents() {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
      keyHandler.handleInput(e);
      mouseHandler.handleEvent(e);

      if (e.type == SDL_MOUSEMOTION) {
        int x = e.motion.x;
        int y = e.motion.y;
      }
    }
    mouseHandler.updateMousePosition();
  }

  bool ScreenHandler::isRunning() const {
    return running;
  }

  CameraHandler* ScreenHandler::getCameraHandler() const {
    return const_cast<CameraHandler*>(&cameraHandler);
  }
  
  SDL_Renderer* ScreenHandler::getRenderer() const {
    return platform.getRenderer();
  }

  SDL_Window* ScreenHandler::getWindow() const {
    return platform.getWindow();
  }
}
