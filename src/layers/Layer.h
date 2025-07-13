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
    explicit Layer(const std::string& name, LayerCategory category, std::shared_ptr<Project::Entities::EntitiesManager> entityManager);
    
    const std::string& getName() const { return name; }
    LayerCategory getCategory() const { return category; }

    bool isActive() const { return active; }
    void setActive(bool value) { active = value; }

    bool isInteractable() const { return interactable; }
    void setInteractable(bool value) { interactable = value; }

    bool followsCamera() const { return followCamera; }
    void setFollowCamera(bool value) { followCamera = value; }

    bool isVisible() const { return visible; }
    void setVisible(bool value) { visible = value; }

    std::shared_ptr<Project::Entities::EntitiesManager> getEntitiesManager() const { return entitiesManager; }
    void setEntitiesManager(std::shared_ptr<Project::Entities::EntitiesManager> mgr) { entitiesManager = std::move(mgr); }

    void update(float deltaTime) override;
    void render() override;

  private:
    std::shared_ptr<Project::Entities::EntitiesManager> entitiesManager;
    LayerCategory category = LayerCategory::CUSTOM;
    std::string name;
    
    bool active = true;
    bool followCamera = true;
    bool interactable = true;
    bool visible = true;
  };
}

#endif
