#ifndef LAYER_H
#define LAYER_H

#include "LayerCategory.h"

#include <string>
#include <memory>

#include "entities/EntitiesManager.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"

namespace Project::Layers {
  class Layer : public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {
  public:
    explicit Layer(const std::string& layerName, LayerCategory layerCategory, std::shared_ptr<Project::Entities::EntitiesManager> entityManager);
    
    const std::string& getName() const { return name; }
    LayerCategory getCategory() const { return category; }

    bool isVisible() const { return visible; }
    void setVisible(bool value) { visible = value; }

    bool isActive() const { return active; }
    void setActive(bool value) { active = value; }

    std::shared_ptr<Project::Entities::EntitiesManager> getEntitiesManager() const { return manager; }
    void setEntitiesManager(std::shared_ptr<Project::Entities::EntitiesManager> mgr) { manager = std::move(mgr); }

    void update(float deltaTime) override;
    void render() override;

  private:
    std::shared_ptr<Project::Entities::EntitiesManager> manager;
    LayerCategory category = LayerCategory::CUSTOM;
    std::string name;
    
    bool visible = true;
    bool active = true;
  };
}

#endif
