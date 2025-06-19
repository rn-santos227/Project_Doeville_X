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

    }

    void remove(const std::string& id) {

    }
  };
}

#endif
