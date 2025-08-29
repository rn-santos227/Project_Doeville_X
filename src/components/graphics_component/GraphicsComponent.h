#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "GraphicsData.h"

#include <cstddef>
#include <cstdint>
#include <future>
#include <string>
#include <vector>

#include <SDL.h>

#include "assets/AssetsManager.h"
#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/TextureHolder.h"
#include "handlers/animation/AnimationHandler.h"
#include "handlers/camera/Camera.h"
#include "handlers/camera/CameraHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "interfaces/render_interface/Material.h"
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
    struct LODLevel {
      std::string assetPath;
      float distance2;
    };
    
    GraphicsComponent(
      SDL_Renderer* renderer,
      Project::Handlers::ResourcesHandler* resourcesHandler,
      Project::Assets::AssetsManager& assetsManager,
      Project::Utilities::LogsManager& logsManager);
    ~GraphicsComponent();

    static void setCameraHandler(Project::Handlers::CameraHandler* handler);
    static Project::Handlers::CameraHandler* getCameraHandler();
    
    ComponentType getType() const override { return ComponentType::GRAPHICS; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;
    
    void setEntityPosition(float x, float y) override;
    void setEntityRotation(float _angle) override { data.rotation = _angle; data.verticesDirty = true; }
    
    bool setTexture(Project::Handlers::ResourcesHandler& resourcesHandler, const std::string& imagePath);
    void setShape(int width, int height, SDL_Color color);
    void setCircle(int radius, SDL_Color color);
    void setColor(SDL_Color color);
    void setPosition(float x, float y, float width, float height);

    void setBorderColor(SDL_Color _color) { data.borderColor = _color; }
    void setBorderWidth(int _width) { data.borderWidth = _width; }
    void setRotationEnabled(bool _enabled) { if (data.rotationEnabled != _enabled) data.verticesDirty = true; data.rotationEnabled = _enabled; }

    void setDepth(int d) { data.depth = d; }
    int getDepth() const { return data.depth; }

    void setBlendMode(SDL_BlendMode mode) { data.blendMode = mode; if (texture) SDL_SetTextureBlendMode(texture, mode); }
    SDL_BlendMode getBlendMode() const { return data.blendMode; }

    void setInstanced(bool inst) { data.instanced = inst; }
    bool isInstanced() const { return data.instanced; }

    bool isRotationEnabled() const { return data.rotationEnabled; }

    float getRotation() const { return data.rotation; }
    void setRotation(float _angle) { if (data.rotation != _angle) data.verticesDirty = true; data.rotation = _angle; }

    SDL_FRect getRect() const;
    int getWidth() const { return static_cast<int>(data.destRect.w); }
    int getHeight() const { return static_cast<int>(data.destRect.h); }
    
    Project::Handlers::AnimationHandler* getAnimationHandler() { return animationHandler.get(); }
    const SDL_Rect& getSrcRect() const { return data.srcRect; }

    SDL_Renderer* getRenderer() const { return renderer; }
    SDL_Texture* getBatchTexture();

    void setMaterial(Project::Interfaces::Material* mat) { material = mat; }
    Project::Interfaces::Material* getMaterial() const { return material; }
    std::uint32_t getMaterialId() const { return material ? material->id : 0; }

    const SDL_FRect& getBoundingBox() const { return boundingBox; }
    void setOccluder(bool value) { occluder = value; }
    bool isOccluder() const { return occluder; }
    bool isInFrustum(const Project::Handlers::Camera& cam) const;
    bool isVisible(const Project::Handlers::Camera& cam, const std::vector<SDL_FRect>& occluders,  int frame) const;
    
  private:
    static Project::Handlers::CameraHandler* cameraHandler;
    static std::size_t getTextureMemory(SDL_Texture* tex);

    SDL_Renderer* renderer;
    SDL_FRect boundingBox{0.0f, 0.0f, 0.0f, 0.0f};
    
    Project::Handlers::ResourcesHandler* resourcesHandler;
    Project::Interfaces::Material* material = nullptr;
    
    Project::Assets::AssetsManager& assetsManager;
    Project::Utilities::LogsManager& logsManager;
    GraphicsData data;

    std::unique_ptr<Project::Handlers::AnimationHandler> animationHandler;

    mutable std::vector<SDL_Vertex> shapeVertices;
    std::future<SDL_Texture*> textureFuture;
    std::vector<LODLevel> lodLevels;
    
    mutable float lastCachedRotation = std::numeric_limits<float>::quiet_NaN();
    mutable float cachedCos = Project::Libraries::Constants::DEFAULT_WHOLE;
    mutable float cachedSin = 0.0f;

    mutable int lastVisibilityFrame = -1;
    int activeLOD = -1;

    mutable bool lastVisibilityResult = false;
    bool lodSwapPending = false;
    bool occluder = false;

    SDL_FRect getRenderRect() const;
    SDL_Texture* getTextureToRender();
    
    bool isInCameraView();
    
    void checkAsyncTextureLoad();
    void renderTexture(SDL_Texture* texture, const SDL_FRect& renderRect);
    void renderShape(const SDL_FRect& renderRect);
    void renderCircleShape(const SDL_FRect& renderRect);
    void renderRectShape(const SDL_FRect& renderRect);
    void renderSimpleRect(const SDL_FRect& renderRect);
    void renderComplexRect(const SDL_FRect& renderRect);
    void renderRectBorder(const SDL_FRect& renderRect);
    void setupShapeIndices();
    void updateRotationCache();
    void updateBoundingBox();
  };
}

#endif
