x = 540
y = 360
z = 0

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = true,
    boxes = {
      { x = 0, y = 0, w = 32, h = 32 }
    }
  },
  
  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    width = 32,
    height = 32,
    color_hex = "808080",
    color_alpha = 255
  },
  
  MotionComponent = {
    component = "MotionComponent",
    active = true,
    speed = 100.0,
    acceleration = 50.0,
    friction = 20.0,
    use_acceleration = true
  },
}

return {
  name = "obstacle"
}