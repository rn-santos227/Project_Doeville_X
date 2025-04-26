#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Components {
  class BaseComponent {
  public:
    explicit BaseComponent(LogsManager& logsManager) : logsManager(logsManager) {}
    virtual ~BaseComponent() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual void onAttach() {}
    virtual void onDetach() {}

  protected:
    LogsManager& logsManager;
  };
}

#endif