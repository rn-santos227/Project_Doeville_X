#ifndef SYSTEM_SCHEDULER_H
#define SYSTEM_SCHEDULER_H

#include <string>
#include <vector>
#include <unordered_map>

#include "interfaces/update_interface/Updatable.h"

namespace Project::Systems {
  class SystemScheduler {
  public:
    void addSystem(const std::string& name, Project::Interfaces::Updatable* system, const std::vector<std::string>& dependencies = {});
    void clear();
    void update(float deltaTime);

  private:
    struct Node {
      std::string name;
      Project::Interfaces::Updatable* system = nullptr;
      std::vector<std::string> deps;
    };

    std::vector<Node> systems;
    std::vector<std::vector<Project::Interfaces::Updatable*>> layers;
    bool dirty = true;

    void resolveOrder();
  };
}

#endif
