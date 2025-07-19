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
      { x = 0, y = 0, w = 32, h = 64 }
    },
    restitution = 0.0,
    friction = 0.0,
    surface = "bounce"
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    width = 32,
    height = 64,
    color_hex = "8B4513",
    color_alpha = 255
  }
}

return {
  name = "wall_v"
}