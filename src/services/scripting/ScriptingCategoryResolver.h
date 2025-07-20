#ifndef SCRIPTING_CATEGORY_RESOLVER_H
#define SCRIPTING_CATEGORY_RESOLVER_H

#include "ScriptingCategory.h"
#include <string>

namespace Project::Services {
  class ScriptingCategoryResolver {
  public:
    static ScriptCategory resolve(const std::string& name);
    static std::string categoryToString(ScriptCategory category);
  };
}

#endif
