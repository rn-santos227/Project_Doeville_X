#ifndef TRANSFORM_COMPONENT_POOL_H
#define TRANSFORM_COMPONENT_POOL_H

#include "helpers/object_pool/ObjectPool.h"
#include "components/transform_component/TransformComponent.h"

namespace Project::Helpers {
  using TransformComponentPool = ObjectPool<Project::Components::TransformComponent>;
}

#endif
