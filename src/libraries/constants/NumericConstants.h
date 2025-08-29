#ifndef NUMERIC_CONSTANTS_H
#define NUMERIC_CONSTANTS_H

#include <cstddef>

namespace Project::Libraries::Constants {
  constexpr int ATLAS_WIDTH = 2048;
  constexpr int ATLAS_HEIGHT = 2048;

  constexpr int ALLOC_64 = 64;
  constexpr int ALLOC_128 = 128;
  constexpr int ALLOC_256 = 256;
  constexpr int ALLOC_1024 = 1024;

  constexpr int DEBUG_COLUMN_SPACING = 20;
  constexpr int DEBUG_FONT_SIZE = 14;
  constexpr int DEBUG_LINE_SPACING = 5;
  constexpr int DEBUG_PREFIX_WIDTH = 8;
  constexpr int DEBUG_TEXT_MARGIN = 10;
  constexpr int DEBUG_TEXT_HEIGHT_OFFSET = 5;
  constexpr int DEBUG_VALUE_WIDTH = 5;
  constexpr int DEBUG_VALUE_COL_OFFSET_FROM_RIGHT = 10;
  constexpr int DEBUG_PREFIX_COL_OFFSET_FROM_RIGHT = 90;
  
  constexpr int DEFAULT_CAMERA_CULL_OFFSET = 200;
  constexpr int DEFAULT_CURSOR_SIZE = 32;
  constexpr int DEFAULT_COMPONENT_SIZE = 32;
  constexpr int DEFAULT_LIGHT_RAY_COUNT = 32;
  constexpr int DEFAULT_FONT_SIZE = 24;
  constexpr int DEFAULT_BASE_FONT_SIZE = 16;
  constexpr int DEFAULT_GRID_SPACING = 32;
  constexpr int DEFAULT_STATE_CACHE_LIMIT = 5;
  constexpr int DEFAULT_MAX_CHANNEL = 255;
  constexpr int DEFAULT_COLLISION_PADDING = 50;
  constexpr int DEFAULT_TEXT_MARGIN = 10;
  constexpr int DEFAULT_TEXT_HEIGHT_OFFSET = 5;
  constexpr int DEFAULT_RENDER_CELL_SIZE = 512;
  constexpr int DEFAULT_MAX_DIM = 2048;
  
  constexpr int INT_ONE = 1;
  constexpr int INT_TEN = 10;
  constexpr int INT_HUNDRED = 100;
  constexpr int INT_THOUSAND = 1000;
  constexpr int INT_TEN_THOUSAND = 10000;
  
  constexpr int MAX_MEMORY_SPACE = 255;
  constexpr int BIT_32 = 32;
  constexpr int BIT_64 = 64;
  constexpr int BIT_255 = 255;

  constexpr size_t SZT_32 = 32;
  constexpr size_t SZT_64 = 64;
  constexpr size_t SZT_255 = 255;

  constexpr size_t LOG_QUEUE_MAX_SIZE = 1000;
  constexpr size_t ENTITY_CHECK_INTERVAL = 60;
  constexpr size_t TEXTURE_TASK_QUEUE_MAX_SIZE = 100;
  constexpr size_t DEFAULT_MAX_CHARS = 255;

  constexpr size_t MAX_PATH_SIZE = 4096;
  constexpr size_t MAX_DATA_SIZE = 10 * 1024 * 1024;

  constexpr size_t DEFAULT_ENTITIES_PER_CHUNK = 32;
  constexpr size_t DEFAULT_INITIAL_CAPACITY = 1000;
  constexpr size_t MAX_SEEDED_ENTITY = 1000;
  
  constexpr size_t ASSET_NAME_ALIAS_COUNT = 7;
  constexpr size_t CAMERA_NAME_ALIAS_COUNT = 13;
  constexpr size_t COMPONENT_NAME_ALIAS_COUNT = 38;
  constexpr size_t DIMENSION_NAME_ALIAS_COUNT = 10;
  constexpr size_t ENTITY_ATTRIBUTE_NAME_ALIAS_COUNT = 25;
  constexpr size_t INPUT_NAME_ALIAS_COUNT = 3;
  constexpr size_t KEY_ACTION_NAME_ALIAS_COUNT = 56;
  constexpr size_t LAYER_NAME_ALIAS_COUNT = 7;
  constexpr size_t MODAL_NAME_ALIAS_COUNT = 4;
  constexpr size_t SCRIPTING_NAME_ALIAS_COUNT = 6;
  constexpr size_t STATE_NAME_ALIAS_COUNT = 24;
  constexpr size_t STYLING_NAME_ALIAS_COUNT = 49;
  constexpr size_t SURFACE_NAME_ALIAS_COUNT = 7;
}

#endif
