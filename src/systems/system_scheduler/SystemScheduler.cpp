#include "SystemScheduler.h"

#include <queue>
#include "utilities/thread/ThreadPool.h"

namespace Project::Systems {
  void SystemScheduler::addSystem(const std::string& name, Project::Interfaces::Updatable* system, const std::vector<std::string>& dependencies) {
    systems.push_back(Node{name, system, dependencies});
    dirty = true;
  }

  void SystemScheduler::clear() {
    systems.clear();
    layers.clear();
    dirty = true;
  }

  void SystemScheduler::resolveOrder() {
    if (!dirty) return;
    layers.clear();
    std::unordered_map<std::string, size_t> indegree;
    std::unordered_map<std::string, std::vector<std::string>> graph;
    for (const auto& node : systems) {
      indegree[node.name];
      for (const auto& dep : node.deps) {
        graph[dep].push_back(node.name);
        ++indegree[node.name];
      }
    }
    std::queue<std::pair<std::string, size_t>> q;
    for (auto& [name, deg] : indegree) {
      if (deg == 0) q.push({name, 0});
    }
    std::unordered_map<std::string, Node*> lookup;
    for (auto& node : systems) lookup[node.name] = &node;
    while (!q.empty()) {
      auto [cur, level] = q.front();
      q.pop();
      if (lookup[cur] && lookup[cur]->system) {
        if (layers.size() <= level) layers.resize(level + 1);
        layers[level].push_back(lookup[cur]->system);
      }
      for (const auto& next : graph[cur]) {
        if (--indegree[next] == 0) q.push({next, level + 1});
      }
    }
    dirty = false;
  }

  void SystemScheduler::update(float deltaTime) {
    resolveOrder();
    auto& pool = Project::Utilities::ThreadPool::getInstance();
    
    for (auto& layer : layers) {
      for (auto* sys : layer) {
        if (!sys) continue;
        pool.enqueue([sys, deltaTime]() { sys->update(deltaTime); });
      }
      pool.wait();
    }
  }
}
