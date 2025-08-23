#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "RendererAPI.h"

namespace Project::Platform {
  class OpenGLRenderer : public RendererAPI {
  public:
    bool init() override;
    void clear() override;
    void present() override;
  };
}

#endif
