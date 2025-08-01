#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include <functional>
#include <memory>
#include <vector>

namespace Project::Helpers {
  template <typename T>
  class ObjectPool {
    using Deleter = std::function<void(T*)>;

    static ObjectPool<T>& getInstance() {
      static ObjectPool<T> instance;
      return instance;
    }

    
  };
}

#endif
