#ifndef SCRIPT_CONSTANTS_H
#define SCRIPT_CONSTANTS_H

#include <SDL.h>

namespace Project::Libraries::Constants {
  constexpr const char* LUA_STATE_SUFFIX = ".state.lua";
  constexpr const char* LUA_LAYER_SUFFIX = ".layer.lua";
  constexpr const char* LUA_ENTITY_SUFFIX = ".entity.lua";
  constexpr const char* LUA_ITEM_SUFFIX = ".item.lua";
  constexpr const char* LUA_ASSET_SUFFIX = ".asset.lua";

  constexpr const char* SCRIPT_LUA_SUFFIX = ".lua";
  constexpr const char* SCRIPT_PYTHON_SUFFIX = ".py";
  constexpr const char* SCRIPT_JAVASCRIPT_SUFFIX = ".js";
  constexpr const char* SCRIPT_TYPESCRIPT_SUFFIX = ".ts";
  constexpr const char* SCRIPT_RUBY_SUFFIX = ".rb";
  constexpr const char* SCRIPT_BASH_SUFFIX = ".sh";
  constexpr const char* SCRIPT_JSON_SUFFIX = ".json";
  constexpr const char* SCRIPT_XML_SUFFIX = ".xml";
  constexpr const char* SCRIPT_YAML_SUFFIX = ".yaml";
  constexpr const char* SCRIPT_TOML_SUFFIX = ".toml";
  constexpr const char* SCRIPT_SQL_SUFFIX = ".sql";
  constexpr const char* SCRIPT_HTML_SUFFIX = ".html";
  constexpr const char* SCRIPT_CSS_SUFFIX = ".css";
  constexpr const char* SCRIPT_TEXT_SUFFIX = ".txt";
  constexpr const char* SCRIPT_INI_SUFFIX = ".ini";

  constexpr const char* SCRIPT_GENERIC_SUFFIX = ".script";
  constexpr const char* ENV_EXTENSION = ".env";
}

#endif
