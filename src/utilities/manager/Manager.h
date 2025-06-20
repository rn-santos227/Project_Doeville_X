#ifndef MANAGER_H
#define MANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>

namespace Project::Utilities {
  template<typename T, typename Ptr = std::shared_ptr<T>>
  class Manager {
  public:
    Manager() = default;
    virtual ~Manager() = default;

    void add(const std::string& id, Ptr object) {
      std::lock_guard<std::mutex> lock(managerMutex);
      objects[id] = std::move(object);
    }

    void remove(const std::string& id) {
      std::lock_guard<std::mutex> lock(managerMutex);
      objects.erase(id);
    }

    void update(float deltaTime) {
      std::lock_guard<std::mutex> lock(managerMutex);
      for (auto& [id, obj] : objects) {
        obj->update(deltaTime);
      }
    }

    void render() {

    }

  protected:
    std::unordered_map<std::string, Ptr> objects;
    std::mutex managerMutex;
  };
}

#endif
