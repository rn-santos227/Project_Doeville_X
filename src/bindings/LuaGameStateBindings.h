#ifndef LUA_GAME_STATE_BINDINGS_H
#define LUA_GAME_STATE_BINDINGS_H

extern "C" {
  #include <lua.h>
}

namespace Project { namespace Assets { class AssetsManager; } }
namespace Project { namespace Factories { class AssetsFactory; } }

namespace Project::Bindings::LuaBindings {
  void setAssetsManager(Project::Assets::AssetsManager* manager);
  void setAssetsFactory(Project::Factories::AssetsFactory* factory);
}

#endif
