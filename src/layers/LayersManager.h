#ifndef LAYERS_MANAGER_H
#define LAYERS_MANAGER_H

#include "Layer.h"
#include "LayerCategory.h"

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include <SDL.h>

#include "interfaces/render_interface/Renderable.h"
#include "interfaces/reset_interface/Resetable.h"
#include "interfaces/update_interface/Updatable.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Entities { class Entity; class EntitiesManager; }
namespace Project::States { class GameState; }

namespace Project::Layers {
  class LayersManager : 
  public Project::Interfaces::Renderable,
  public Project::Interfaces::Resetable,  
  public Project::Interfaces::Updatable {
  public:
    LayersManager() = default;
    ~LayersManager() = default;

    void update(float deltaTime) override;
    void render() override;
    void reset();

    void addLayer(Layer layer);
    void addLayer(LayerCategory category);
    void addLayer(const std::string& name, LayerCategory category = LayerCategory::CUSTOM);
    void removeLayer(const std::string& name);
    void renderVisionMask(SDL_Renderer* renderer);

    bool hasLayer(const std::string& name) const;
    bool hasLayer(LayerCategory category) const;

    std::shared_ptr<Project::Entities::EntitiesManager> getLayer(const std::string& name);
    std::shared_ptr<Project::Entities::EntitiesManager> getLayer(LayerCategory category);
    std::shared_ptr<Project::Entities::EntitiesManager> getFirstLayer();
    std::shared_ptr<Project::Entities::EntitiesManager> getLastLayer();
    
    std::shared_ptr<Project::Entities::Entity> findEntity(const std::string& name);
    void setGameState(Project::States::GameState* state);
    Project::States::GameState* getGameState() const { return gameState; }

    void setLayerActive(const std::string& name, bool active);
    void setFollowCamera(const std::string& name, bool active);
    void setLayerInteractable(const std::string& name, bool active);
    void setLayerVisible(const std::string& name, bool visible);
    void setLayerDarkness(const std::string& name, float value);
    void setLogsManager(Project::Utilities::LogsManager* manager);

    size_t getTotalEntityCount() const;
    
    void clampEntitiesToRect(const SDL_Rect& rect);
    void warpEntitiesAcrossRect(const SDL_Rect& rect);

  private:
    Project::Utilities::LogsManager* logsManager = nullptr;
    Project::States::GameState* gameState = nullptr;
    
    std::vector<Layer> layers;

    int categoryOrder(LayerCategory category) const;
    bool hasActiveCinematic() const;
  };
}

#endif
