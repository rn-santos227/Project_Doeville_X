#ifndef RESOURCE_CLEANER_H
#define RESOURCE_CLEANER_H

#include <utility>

namespace Project::Helpers {
  struct ResourceCleaner {
    template<typename AssocContainer, typename Deleter>
    static void cleanupMap(AssocContainer& container, Deleter deleter) {
      for (auto& pair : container) {
        if (pair.second) {
          deleter(pair.second);
        }
      }
      container.clear();
    }
  };
}

#endif
