#ifndef LAYER_CATEGORY_H
#define LAYER_CATEGORY_H

namespace Project::Layers {
  enum class LayerCategory {
    HUD,
    OVERLAY,
    FOREGROUND,
    MIDGROUND,
    BACKGROUND,
    CINEMATIC,
    CUSTOM
  };
}

#endif
