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
#include "services/styling/StyleProperty.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Components {
  class GraphicsComponent : public BaseComponent, public PositionableComponent, public TextureHolder, public Project::Interfaces::Rotatable, public Project::Interfaces::Stylable  {
  public:
    GraphicsComponent(SDL_Renderer* renderer, Project::Handlers::ResourcesHandler* resourcesHandler, Project::Utilities::LogsManager& logsManager);
    ~GraphicsComponent();

    static void setCameraHandler(Project::Handlers::CameraHandler* handler);
    static Project::Handlers::CameraHandler* getCameraHandler();
    
    ComponentType getType() const override { return ComponentType::GRAPHICS; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;
    
    void setEntityPosition(int x, int y) override;
    void setEntityRotation(float _angle) override { rotation = _angle; verticesDirty = true; }
    
    bool setTexture(Project::Handlers::ResourcesHandler& resourcesHandler, const std::string& imagePath);
    void setShape(int width, int height, SDL_Color color);
    void setCircle(int radius, SDL_Color color);
    void setPosition(int x, int y, int width, int height);

    void setBorderColor(SDL_Color _color) { borderColor = _color; }    
    void setBorderWidth(int _width) { borderWidth = _width; }
    void setRotationEnabled(bool _enabled) { if (rotationEnabled != _enabled) verticesDirty = true; rotationEnabled = _enabled; }

    bool isRotationEnabled() const { return rotationEnabled; }
    
    float getRotation() const { return rotation; }
    void setRotation(float _angle) { rotation = _angle; }

    const SDL_Rect& getRect() const { return destRect; }
    int getWidth() const { return destRect.w; }
    int getHeight() const { return destRect.h; }
    
    Project::Handlers::AnimationHandler* getAnimationHandler() { return animationHandler.get(); }

  private:
    static Project::Handlers::CameraHandler* cameraHandler;

    SDL_Renderer* renderer;
    
    Project::Handlers::ResourcesHandler* resourcesHandler;
    Project::Utilities::LogsManager& logsManager;
    
    Project::Services::GradientType gradient = Project::Services::GradientType::NONE;
    Project::Services::Style style{};

    std::unique_ptr<Project::Handlers::AnimationHandler> animationHandler;
    
    SDL_Color shapeColor = Project::Libraries::Constants::DEFAULT_SHAPE_COLOR;
    SDL_Color borderColor{0, 0, 0, 0};
    SDL_Color gradientStart{0, 0, 0, 0};
    SDL_Color gradientEnd{0, 0, 0, 0};
    
    SDL_Rect destRect{0, 0, 0, 0};

    mutable std::vector<SDL_Vertex> shapeVertices;
    std::future<SDL_Texture*> textureFuture;
    std::vector<int> shapeIndices;

    std::string pendingTexturePath;
    int borderWidth{0};
    
    mutable float lastCachedRotation = std::numeric_limits<float>::quiet_NaN();
    mutable float cachedCos = Project::Libraries::Constants::DEFAULT_WHOLE;
    mutable float cachedSin = 0.0f;
    float radius = 0.0f;
    float rotation = 0.0f;

    mutable bool verticesDirty{true};
    bool rotationEnabled = false;
    bool isCircle = false;
    bool drawShape = false;
    bool useGradient = false;

    SDL_Rect getRenderRect() const;
    SDL_Texture* getTextureToRender();
    
    bool isInCameraView();
    void checkAsyncTextureLoad();
    void renderTexture(SDL_Texture* texture, const SDL_Rect& renderRect);
    void renderShape(const SDL_Rect& renderRect);
    void renderCircleShape(const SDL_Rect& renderRect);
    void renderRectShape(const SDL_Rect& renderRect);
    void renderSimpleRect(const SDL_Rect& renderRect);
    void renderComplexRect(const SDL_Rect& renderRect);
    void renderRectBorder(const SDL_Rect& renderRect);
    
    void updateRotationCache();
    void setupShapeIndices();
  };
}

#endif
