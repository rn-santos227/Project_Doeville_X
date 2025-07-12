#ifndef LAYER_H
#define LAYER_H

#include <string>
#include <memory>

#include "entities/EntitiesManager.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"

namespace Project::Layers {
  class Layer : public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {

  };
}

#endif
