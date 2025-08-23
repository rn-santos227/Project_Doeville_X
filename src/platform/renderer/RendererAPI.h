#ifndef RENDERER_API_H
#define RENDERER_API_H

namespace Project::Platform {
  enum class RendererType { OpenGL, Vulkan, DirectX, Metal };

  class RendererAPI {
  public:
    virtual ~RendererAPI() = default;
    virtual bool init() = 0;
    virtual void clear() = 0;
    virtual void present() = 0;
  };
}

#endif
