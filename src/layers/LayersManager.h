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

    void addLayer(const std::string& name, LayerCategory category = LayerCategory::CUSTOM);
    void addLayer(LayerCategory category);

    bool hasLayer(const std::string& name) const;
    bool hasLayer(LayerCategory category) const;

    void setLayerVisible(const std::string& name, bool visible);
    void setLayerActive(const std::string& name, bool active);
    void removeLayer(const std::string& name);

    std::shared_ptr<Project::Entities::EntitiesManager> getLayer(const std::string& name);
    std::shared_ptr<Project::Entities::EntitiesManager> getLayer(LayerCategory category);
    std::shared_ptr<Project::Entities::EntitiesManager> getFirstLayer();
    
  private:
    std::vector<Layer> layers;
  };
}

#endif
