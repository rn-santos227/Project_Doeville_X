#include "SystemScheduler.h"

#include <queue>

namespace Project::Systems {
  void SystemScheduler::addSystem(const std::string& name,
                                  Project::Interfaces::Updatable* system,
                                  const std::vector<std::string>& dependencies) {
    systems.push_back(Node{name, system, dependencies});
    dirty = true;
  }

  void SystemScheduler::clear() {
    systems.clear();
    ordered.clear();
    dirty = true;
  }

  void SystemScheduler::resolveOrder() {
    if (!dirty) return;
    ordered.clear();
    std::unordered_map<std::string, size_t> indegree;
    std::unordered_map<std::string, std::vector<std::string>> graph;
    for (const auto& node : systems) {
      indegree[node.name];
      for (const auto& dep : node.deps) {
        graph[dep].push_back(node.name);
        ++indegree[node.name];
      }
    }
    std::queue<std::string> q;
    for (auto& [name, deg] : indegree) {
      if (deg == 0) q.push(name);
    }
    std::unordered_map<std::string, Node*> lookup;
    for (auto& node : systems) lookup[node.name] = &node;
    while (!q.empty()) {
      std::string cur = q.front(); q.pop();
      if (lookup[cur] && lookup[cur]->system) {
        ordered.push_back(lookup[cur]->system);
      }
      for (const auto& next : graph[cur]) {
        if (--indegree[next] == 0) q.push(next);
      }
    }
    dirty = false;
  }

  void SystemScheduler::update(float deltaTime) {
    resolveOrder();
    for (auto* sys : ordered) {
      if (sys) sys->update(deltaTime);
    }
  }
}
