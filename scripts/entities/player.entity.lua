x = 640
y = 360
z = 0

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = false,
    boxes = {
      { x = 0, y = 0, w = 32, h = 32 }
    }
  },
  
  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    width = 32,
    height = 32,
    color_hex = "ADD8E6",
    color_alpha = 255
  }
  
  KeysComponent = {
    component = "KeysComponent",
    active = true,
    keys = {
      { key = "w", action = "move_up" },
      { key = "a", action = "move_left" },
      { key = "s", action = "move_down" },
      { key = "d", action = "move_right" }
    }
  }
  
  MotionComponent = {
    component = "MotionComponent",
    active = true,
    speed = 100.0
  },
}

return {
  name = "player"
}