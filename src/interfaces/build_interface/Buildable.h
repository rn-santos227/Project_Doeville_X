#ifndef BUILDABLE_H
#define BUILDABLE_H

#include <string>

#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Interfaces {
  class Buildable {
  public:
    virtual ~Buildable() = default;
    virtual void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) = 0;
  };
}

#endif
