x = 0
y = 0
z = 0

group = "gameplay"

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    surface = "destroy_on_hit",
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
    brake_power = 0.0,
    speed = 800.0,
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

function update(deltaTime)
  local target = getCollidedEntity(id, {"box", "ball"})
  if target then
    destroyEntity(target)
  end
end

return {
  name = "bullet_p"
}
