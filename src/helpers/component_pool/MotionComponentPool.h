#ifndef MOTION_COMPONENT_POOL_H
#define MOTION_COMPONENT_POOL_H

#include "helpers/object_pool/ObjectPool.h"
#include "components/motion_component/MotionComponent.h"

namespace Project::Helpers {
  using MotionComponentPool = ObjectPool<Project::Components::MotionComponent>;
}

#endif
