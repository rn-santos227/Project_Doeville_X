#ifndef MATERIAL_H
#define MATERIAL_H

#include <cstdint>
#include <vector>
#include <SDL.h>

namespace Project::Interfaces {
  struct Material {
  public:
    std::uint32_t id;
    std::uint32_t shaderProgramId = 0;
    std::vector<SDL_Texture*> textures;
    std::uint32_t renderFlags = 0;

  private:
    inline static std::uint32_t nextId = 0;
  };
}

#endif
