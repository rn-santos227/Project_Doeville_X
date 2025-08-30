x = 400
y = 360
z = 0

group = "gameplay"

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = true,
    boxes = {
      { x = 0, y = 0, w = 128, h = 32 }
    },
    restitution = 1.0,
    friction = 0.0,
    surface = "bounce"
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    occluder = true
  }
}

return {
  name = "wall_h"
}
