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
  },
  
  KeysComponent = {
    component = "KeysComponent",
    active = true,
    keys = {
      { key = "up", action = "move_up" },
      { key = "left", action = "move_left" },
      { key = "down", action = "move_down" },
      { key = "right", action = "move_right" },
      { key = "z", action = "action_1" },
    },
    bindings = {
      action_1 = "action_1"
    }
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

function action_1()
  print("Action 1 placeholder")
end

return {
  name = "player"
}