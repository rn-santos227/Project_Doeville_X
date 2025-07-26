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
      { x = 4, y = 4, radius = 4 }
    },
  },

  MotionComponent = {
    component = "MotionComponent",
    active = true,
    use_acceleration = false,
    rotation = true,
    brake_power = 0.5,
    speed = 350.0,
    friction = 0.0
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    class = "bullet",
    active = true,
    rotation = true,
    radius = 4
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
    seconds = 3,
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
