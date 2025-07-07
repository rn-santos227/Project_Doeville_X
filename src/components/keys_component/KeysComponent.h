#ifndef KEYS_COMPONENT_H
#define KEYS_COMPONENT_H

#include "components/BaseComponent.h"

#include <string>
#include <unordered_map>

#include "entities/Entity.h"
#include "handlers/input/KeyHandler.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class KeysComponent : public BaseComponent {
  public:
    KeysComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::KeyHandler* keyHanndler);
    ~KeysComponent() override = default;

    void update(float deltaTime) override;

    bool isActionTriggered(Project::Handlers::KeyAction action) const;
    bool isKeyPressed(SDL_Scancode key) const;

    void addActionCallback(Project::Handlers::KeyAction action, const std::string& functionName);
    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    
  private:
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    Project::Entities::Entity* owner = nullptr;
    std::unordered_map<Project::Handlers::KeyAction, std::string> actionCallbacks;
  };
}
#endif
