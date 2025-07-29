#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

extern "C" {
  #include <lua.h>
}

namespace Project::Bindings {
  namespace LuaBindings {
    //Gamestate Bindings
    int lua_addEntityToSeed(lua_State* L);
    int lua_changeState(lua_State* L);
    int lua_resetState(lua_State* L);
    int lua_exitGame(lua_State* L);
    int lua_spawnEntity(lua_State* L);
    int lua_setActiveCamera(lua_State* L);
    int lua_setBackgroundColor(lua_State* L);
    int lua_setBackgroundImage(lua_State* L);
    int lua_setPlayerEntity(lua_State* L);
    int lua_startEntitySeeder(lua_State* L);

    // EntitiesManager Bindings
    int lua_getCollidedEntity(lua_State* L);
    int lua_getEntityDetails(lua_State* L);
    int lua_getEntitySpeed(lua_State* L);
    int lua_getEntityVelocity(lua_State* L);
    int lua_setEntityText(lua_State* L);
    int lua_setTimerActive(lua_State* L);
    int lua_addNumericValue(lua_State* L);
    int lua_setNumericValue(lua_State* L);
    int lua_getNumericValue(lua_State* L);
    int lua_stopTimer(lua_State* L);
    int lua_brakeEntity(lua_State* L);
    int lua_turnLeft(lua_State* L);
    int lua_turnRight(lua_State* L);
    int lua_isActionPressed(lua_State* L);
    int lua_destroyEntity(lua_State* L);
    int lua_destroyEntities(lua_State* L);
    int lua_destroySelf(lua_State* L);

    // EntitiesFactory bindings
    int lua_factoryChangeState(lua_State* L);

    // SpawnerComponent bindings
    int lua_spawn(lua_State* L);
  }
}

#endif
