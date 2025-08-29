#include "MapAsset.h"

#include <fstream>
#include <sstream>

#include "libraries/keys/LuaAssetKeys.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Assets {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Handlers::ResourcesHandler;

  namespace Keys = Project::Libraries::Keys;
  MapAsset::MapAsset(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : BaseAsset(renderer, logsManager, resourcesHandler) {}

  bool MapAsset::loadFromLua(const std::string& scriptPath, const std::string& assetName) {
    LuaStateWrapper lua(logsManager);
    if (!lua.loadScript(scriptPath)) {
      logsManager.logError("Failed to load asset script: " + scriptPath);
      return false;
    }

    lua_State* L = lua.get();
    lua_getglobal(L, Keys::LUA_ASSET_GET_ASSET);
    if (!lua_isfunction(L, -1)) {
      logsManager.logError("Asset script missing getAsset function: " + scriptPath);
      return false;
    }

    if (!assetName.empty()) {
      lua_pushstring(L, assetName.c_str());
    }

    if (lua_pcall(L, assetName.empty() ? 0 : 1, 1, 0) != LUA_OK) {
      const char* msg = lua_tostring(L, -1);
      logsManager.logError(std::string("Error executing getAsset: ") + (msg ? msg : "unknown"));
      return false;
    }

    if (!lua_istable(L, -1)) {
      logsManager.logError("getAsset must return a table");
      lua_pop(L, 1);
      return false;
    }

    lua_getfield(L, -1, Keys::LUA_ASSET_NAME);
    if (lua_isstring(L, -1)) data.name = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_TAG);
    if (lua_isstring(L, -1)) data.tag = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_MAP);
    if (lua_istable(L, -1)) {
      size_t len = lua_rawlen(L, -1);
      for (size_t i = 1; i <= len; ++i) {
        lua_rawgeti(L, -1, i);
        std::vector<int> rowValues;
        if (lua_isstring(L, -1)) {
          std::string row = lua_tostring(L, -1);
          std::istringstream iss(row);
          int v;
          while (iss >> v) {
            rowValues.push_back(v);
          }
        } else if (lua_istable(L, -1)) {
          size_t innerLen = lua_rawlen(L, -1);
          for (size_t j = 1; j <= innerLen; ++j) {
            lua_rawgeti(L, -1, j);
            if (lua_isnumber(L, -1)) {
              rowValues.push_back(static_cast<int>(lua_tointeger(L, -1)));
            }
            lua_pop(L, 1);
          }
        }

        if (!rowValues.empty()) {
          if (static_cast<int>(rowValues.size()) > mapData.cols) {
            mapData.cols = static_cast<int>(rowValues.size());
          }
          mapData.layout.push_back(rowValues);
        }
        lua_pop(L, 1);
      }
      mapData.rows = static_cast<int>(mapData.layout.size());
    }
    lua_pop(L, 1);
    
    if (mapData.layout.empty()) {
      lua_getfield(L, -1, Keys::LUA_ASSET_MAP_FILE);
      if (lua_isstring(L, -1)) {
        std::string filePath = lua_tostring(L, -1);
        std::ifstream file(filePath);
        if (!file.is_open()) {
          logsManager.logError("Failed to open map file: " + filePath);
        } else {
          std::string line;
          while (std::getline(file, line)) {
            std::istringstream iss(line);
            int v;
            std::vector<int> rowValues;
            while (iss >> v) {
              rowValues.push_back(v);
            }
            if (!rowValues.empty()) {
              if (static_cast<int>(rowValues.size()) > mapData.cols) {
                mapData.cols = static_cast<int>(rowValues.size());
              }
              mapData.layout.push_back(rowValues);
            }
          }
          mapData.rows = static_cast<int>(mapData.layout.size());
        }
      }
      lua_pop(L, 1);
    }
  
    lua_getfield(L, -1, Keys::LUA_ASSET_TILES);
    if (lua_istable(L, -1)) {
      lua_pushnil(L);
      while (lua_next(L, -2) != 0) {
        if (lua_isnumber(L, -2) && lua_isstring(L, -1)) {
          int key = static_cast<int>(lua_tointeger(L, -2));
          const char* value = lua_tostring(L, -1);
          mapData.tiles[key] = value;
        }
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);

    lua_pop(L, 1);
    data.category = AssetCategory::MAP;
    return !mapData.layout.empty();
  }
}
