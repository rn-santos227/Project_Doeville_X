x = 0
y = 0
z = 0

group = "gameplay"

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = true,
    boxes = {
      { x = 0, y = 0, w = 32, h = 32 }
    },
    restitution = 0.0,
    friction = 0.2
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

  PhysicsComponent = {
    component = "PhysicsComponent",
    active = true,
    friction = 0.2,
    restitution = 0.0,
    weight = 1.0,
    density = 0.2
  }
}

return {
  name = "obstacle"
}
