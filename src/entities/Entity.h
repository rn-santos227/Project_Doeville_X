#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Entities {
  class Entity {

  };
}

#endif