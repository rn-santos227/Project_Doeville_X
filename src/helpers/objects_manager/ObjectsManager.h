#ifndef MANAGER_H
#define MANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>

namespace Project::Helpers {
  template<typename T, typename Ptr = std::shared_ptr<T>>
  class ObjectsManager {
  public:
    ObjectsManager() = default;
    virtual ~ObjectsManager() = default;

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
      std::lock_guard<std::mutex> lock(managerMutex);
      for (auto& [id, obj] : objects) {
        obj->render();
      }
    }

    size_t count() const {
      std::lock_guard<std::mutex> lock(managerMutex);
      return objects.size();
    }

    std::unordered_map<std::string, Ptr>& getObjects() { return objects; }
    const std::unordered_map<std::string, Ptr>& getObjects() const { return objects; }

  protected:
    std::unordered_map<std::string, Ptr> objects;
    mutable std::mutex managerMutex;
  };
}

#endif
