#ifndef METER_COMPONENT_H
#define METER_COMPONENT_H

#include "MeterData.h"

#include <string>

#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "interfaces/style_interface/Stylable.h"

namespace Project { namespace Entities { class Entity; } }
namespace Project::Components { class NumericComponent; }

namespace Project::Components {
  class MeterComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Stylable {

  };
}

#endif
