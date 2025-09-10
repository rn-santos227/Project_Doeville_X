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

  int lua_addEntityToSeed(lua_State* L);
  int lua_changeState(lua_State* L);
  int lua_resetState(lua_State* L);
  int lua_exitGame(lua_State* L);
  int lua_spawnEntity(lua_State* L);
  int lua_setActiveCamera(lua_State* L);
  int lua_cameraZoomIn(lua_State* L);
  int lua_cameraZoomOut(lua_State* L);
}

#endif
