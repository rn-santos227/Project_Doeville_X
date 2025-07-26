#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

#include <string>

#include "interfaces/build_interface/Buildable.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project { namespace Entities { class Entity; } }
namespace Project::Components {
  class BaseComponent : public Project::Interfaces::Buildable, public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {
  public:
    explicit BaseComponent(Project::Utilities::LogsManager& logsManager) : logsManager(logsManager) {}
    virtual ~BaseComponent() = default;

    virtual Project::Entities::Entity* getOwner() const { return nullptr; }

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) = 0;

    virtual void onAttach() {}
    virtual void onDetach() {}

    void setClass(const std::string& _classs) { componentClass = _classs; }
    const std::string& getClass() const { return componentClass; }

    void setActive(bool _active) { active = _active; }
    bool isActive() const { return active; }

  protected:
    Project::Utilities::LogsManager& logsManager;
    std::string componentClass;
    bool active = true;
  };
}

#endif
