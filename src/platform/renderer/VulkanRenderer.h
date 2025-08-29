#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "RendererAPI.h"

namespace Project::Platform {
  class VulkanRenderer : public RendererAPI {
  public:
    bool init() override;
    void clear() override;
    void present() override;
  };
}

#endif
