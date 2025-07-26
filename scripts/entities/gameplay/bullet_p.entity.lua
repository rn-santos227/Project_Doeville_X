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
      { x = 0, y = 0, w = 8, h = 8 }
    }
  },

  MotionComponent = {
    component = "MotionComponent",
    active = true,
    brake_power = 0.5,
    speed = 250.0,
    friction = 0.0,
    use_acceleration = false,
    rotation = true,
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    rotation = true
  },

  PhysicsComponent = {
    component = "PhysicsComponent",
    active = true,
    kinematic = true,
    gravity = false,
    friction = 0.0,
    density = 0.1,
    mass = 0.1,
    restitution = 0.0,
    surface = "destroy_on_hit"
  },

  TimerComponent = {
    component = "TimerComponent",
    active = true,
    seconds = 0.6,
    callbacks = "expire"
  }
}

function expire()
  if id then
    destroyEntity(id)
  end
end

return {
  name = "bullet_p"
}
