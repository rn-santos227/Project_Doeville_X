#ifndef LAYERS_MANAGER_H
#define LAYERS_MANAGER_H

#include "Layer.h"
#include "LayerCategory.h"

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"

namespace Project::Layers {
  class LayersManager : public Project::Interfaces::Renderable,  public Project::Interfaces::Updatable {
  public:
    LayersManager() = default;
    ~LayersManager() = default;

    void update(float deltaTime) override;
    void render() override;

    void addLayer(Layer layer);
    void addLayer(LayerCategory category);
    void addLayer(const std::string& name, LayerCategory category = LayerCategory::CUSTOM);
    void removeLayer(const std::string& name);

    bool hasLayer(const std::string& name) const;
    bool hasLayer(LayerCategory category) const;

    std::shared_ptr<Project::Entities::EntitiesManager> getLayer(const std::string& name);
    std::shared_ptr<Project::Entities::EntitiesManager> getLayer(LayerCategory category);
    std::shared_ptr<Project::Entities::EntitiesManager> getFirstLayer();
    std::shared_ptr<Project::Entities::EntitiesManager> getLastLayer();

    void setLayerActive(const std::string& name, bool active);
    void setFollowCamera(const std::string& name, bool active);
    void setLayerInteractable(const std::string& name, bool active);
    void setLayerVisible(const std::string& name, bool visible);

  private:
    int categoryOrder(LayerCategory category) const;
    bool hasActiveCinematic() const;
    std::vector<Layer> layers;
  };
}

#endif
