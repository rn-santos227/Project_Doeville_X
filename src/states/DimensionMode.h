#ifndef DIMENSION_MODE_H
#define DIMENSION_MODE_H

namespace Project::States {
  enum class DimensionMode {
    BOXED,
    BOUNDED,
    FREE_ROAMING,
    HORIZONTAL_SCROLL,
    ISOMETRIC,
    LOOPING,
    MAPPED,
    THREE_DIMENSION,
    VERTICAL_SCROLL,
    WRAPPING
  };
}

#endif
