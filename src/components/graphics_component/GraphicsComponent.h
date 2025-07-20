#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include <future>
#include <string>

#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/TextureHolder.h"
#include "handlers/animation/AnimationHandler.h"
#include "handlers/camera/CameraHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "interfaces/rotation_interface/Rotatable.h"
#include "interfaces/style_interface/Stylable.h"
#include "libraries/constants/Constants.h"
#include "services/styling/Style.h"
#include "services/styling/StyleManager.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Components {
  class GraphicsComponent : public BaseComponent, public PositionableComponent, public TextureHolder, public Project::Interfaces::Rotatable, public Project::Interfaces::Stylable  {
  public:
    GraphicsComponent(SDL_Renderer* renderer, Project::Handlers::ResourcesHandler* resourcesHandler, Project::Utilities::LogsManager& logsManager);
    ~GraphicsComponent();

    static void setCameraHandler(Project::Handlers::CameraHandler* handler);
    static Project::Handlers::CameraHandler* getCameraHandler();
    
    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;

    bool setTexture(Project::Handlers::ResourcesHandler& resourcesHandler, const std::string& imagePath);
    void setShape(int width, int height, SDL_Color color);
    void setCircle(int radius, SDL_Color color);
    void setPosition(int x, int y, int width, int height);
    void setEntityPosition(int x, int y) override;
    
    void setEntityRotation(float angle) override { rotation = angle; }
    void setRotationEnabled(bool enabled) { rotationEnabled = enabled; }
    bool isRotationEnabled() const { return rotationEnabled; }
    
    float getRotation() const { return rotation; }
    void setRotation(float angle) { rotation = angle; }

    const SDL_Rect& getRect() const { return destRect; }
    int getWidth() const { return destRect.w; }
    int getHeight() const { return destRect.h; }
    
    Project::Handlers::AnimationHandler* getAnimationHandler() { return animationHandler.get(); }

  private:
    static Project::Handlers::CameraHandler* cameraHandler;

    SDL_Renderer* renderer;
    Project::Handlers::ResourcesHandler* resourcesHandler;
    Project::Utilities::LogsManager& logsManager;
    Project::Services::Style style{};

    std::unique_ptr<Project::Handlers::AnimationHandler> animationHandler;

    SDL_Color shapeColor = Project::Libraries::Constants::DEFAULT_SHAPE_COLOR;
    SDL_Rect destRect{0, 0, 0, 0};

    float radius = 0;
    float rotation = 0.0f;

    bool rotationEnabled = false;
    bool isCircle = false;
    bool drawShape = false;

    std::future<SDL_Texture*> textureFuture;
    std::string pendingTexturePath;
  };
}

#endif
