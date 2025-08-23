#ifndef PLATFORM_RENDERER_API_H
#define PLATFORM_RENDERER_API_H

namespace Project::Platform {
  enum class RendererType { OpenGL, Vulkan, DirectX, Metal };

  class RendererAPI {
  public:
    virtual ~RendererAPI() = default;
    virtual bool init() = 0;
  };
}

#endif
