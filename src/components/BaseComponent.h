#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

#include "interfaces/build_interface/Buildable.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Components {
  class BaseComponent : public Project::Interfaces::Buildable, public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {
  public:
    explicit BaseComponent(Project::Utilities::LogsManager& logsManager) : logsManager(logsManager) {}
    virtual ~BaseComponent() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) = 0;

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
