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
    friction = 0.2,
    rotation = true,
    restitution = 0.0
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    width = 32,
    height = 32,
    color_hex = "808080",
    rotation = true,
    color_alpha = 255
  },

  MotionComponent = {
    component = "MotionComponent",
    active = true,
    speed = 100.0,
    acceleration = 50.0,
    friction = 20.0,
    use_acceleration = true,
    rotation = true,
  },

  PhysicsComponent = {
    component = "PhysicsComponent",
    active = true,
    rotation = true,
    static = false,
    kinematic = false,
    density = 0.2,
    friction = 0.2,
    mass = 1.0,
    gravity_scale = 1.0,
    restitution = 0.0
  }
}

return {
  name = "obstacle"
}
