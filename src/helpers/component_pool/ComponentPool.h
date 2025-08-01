#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include "helpers/object_pool/ObjectPool.h"

namespace Project::Helpers {
  template <typename T>
  using ComponentPool = Project::Helpers::ObjectPool<T>;
}

#endif
