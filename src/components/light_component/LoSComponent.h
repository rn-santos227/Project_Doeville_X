#ifndef LOS_COMPONENT_H
#define LOS_COMPONENT_H

#include "LoSData.h"

#include <vector>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "utilities/logs_manager/LogsManager.h"
#include "entities/EntitiesManager.h"

namespace Project::Components {
  class LoSComponent : public BaseComponent, public PositionableComponent {

  };
}

#endif
