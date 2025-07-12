x = 400
y = 360
z = 0

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = true,
    boxes = {
      { x = 0, y = 0, w = 64, h = 32 }
    },
    restitution = 0.0,
    friction = 0.2
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    width = 64,
    height = 32,
    color_hex = "8B4513",
    color_alpha = 255
  }
}

return {
  name = "wall"
}