#ifndef NUMERIC_CONSTANTS_H
#define NUMERIC_CONSTANTS_H

#include <cstddef>

namespace Project::Libraries::Constants {
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
  constexpr int DEFAULT_FONT_SIZE = 24;
  constexpr int DEFAULT_BASE_FONT_SIZE = 16;
  constexpr int DEFAULT_GRID_SPACING = 32;
  constexpr int DEFAULT_STATE_CACHE_LIMIT = 5;
  constexpr int DEFAULT_DOUBLE = 2;
  constexpr int DEFAULT_MAX_CHANNEL = 255;
  constexpr int DEFAULT_COLLISION_PADDING = 50;
  
  constexpr int INT_ONE = 1;
  constexpr int INT_TEN = 10;
  constexpr int INT_HUNDRED = 100;
  constexpr int INT_THOUSAND = 1000;
  constexpr int INT_TEN_THOUSAND = 10000;
  constexpr int MAX_MEMORY_SPACE = 255;
  
  constexpr int BIT_32 = 32;
  constexpr int BIT_255 = 255;

  constexpr size_t LOG_QUEUE_MAX_SIZE = 1000;
  constexpr size_t ENTITY_CHECK_INTERVAL = 60;
  constexpr size_t TEXTURE_TASK_QUEUE_MAX_SIZE = 100;

  constexpr size_t MAX_PATH_SIZE = 4096;
  constexpr size_t MAX_DATA_SIZE = 10 * 1024 * 1024;
}

#endif
