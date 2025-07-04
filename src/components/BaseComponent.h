#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

#include "utilities/logs_manager/LogsManager.h"
namespace Project::Components {
  class BaseComponent {
  public:
    explicit BaseComponent(Project::Utilities::LogsManager& logsManager) : logsManager(logsManager) {}
    virtual ~BaseComponent() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual void onAttach() {}
    virtual void onDetach() {}

    void setActive(bool isActive) { active = isActive; }
    bool isActive() const { return active; }

  protected:
    Project::Utilities::LogsManager& logsManager;
    bool active = true;
  };
}

#endif
