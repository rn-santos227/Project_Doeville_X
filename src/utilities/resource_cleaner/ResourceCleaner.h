#ifndef RESOURCE_CLEANER_H
#define RESOURCE_CLEANER_H

#include <utility>

namespace Project::Utilities {
  struct ResourceCleaner {
    template<typename AssocContainer, typename Deleter>
    static void cleanupMap(AssocContainer& container, Deleter deleter) {
      for (auto& pair : container) {

      }
    }
  };
}

#endif
