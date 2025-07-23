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
  
  constexpr int INT_ONE = 1;

  constexpr size_t LOG_QUEUE_MAX_SIZE = 1000;
}

#endif
