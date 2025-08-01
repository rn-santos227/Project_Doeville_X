#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include "entities/Entity.h"
#include "helpers/object_pool/ObjectPool.h"

namespace Project::Helpers {
  using EntityPool = Project::Helpers::ObjectPool<Project::Entities::Entity>;
}

#endif
