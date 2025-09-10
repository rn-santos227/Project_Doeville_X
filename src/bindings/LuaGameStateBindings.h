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
  int lua_setBackgroundColor(lua_State* L);
  int lua_setBackgroundImage(lua_State* L);
  int lua_cameraShake(lua_State* L);
  int lua_cameraRotate(lua_State* L);
  int lua_cameraZoomIn(lua_State* L);
  int lua_cameraZoomOut(lua_State* L);
  int lua_changeState(lua_State* L);
  int lua_exitGame(lua_State* L);
  int lua_loadMapAsset(lua_State* L);
  int lua_resetState(lua_State* L);
  int lua_setActiveCamera(lua_State* L);
  int lua_setDarkness(lua_State* L);
  int lua_setDayLapse(lua_State* L);
  int lua_setLayerDarkness(lua_State* L);
  int lua_setMapSize(lua_State* L);
  int lua_setPlayerEntity(lua_State* L);
  int lua_setTimeCycleActive(lua_State* L);
  int lua_setTimeDarknessLayer(lua_State* L);
  int lua_setTimeOfDay(lua_State* L);
  int lua_spawnEntity(lua_State* L);
  int lua_startEntitySeeder(lua_State* L);
}

#endif
