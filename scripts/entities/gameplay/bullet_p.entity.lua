x = 0
y = 0
z = 0

group = "gameplay"

attributes = {"DISPOSABLE",  "UNBOUNDED"}

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    surface = "ghost_pass",
    active = true,
    solid = false,
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
  destroySelf()
end

function update(deltaTime)
  ignoreCollisionsWith(id, {"player"})

  if getCollidedEntity(id, {"wall_h", "wall_v"}) then
    destroySelf()
    return
  end

  local seen = {}
  local target = getCollidedEntity(id, {"box", "ball"})
  
  local count = 0
  while target and not seen[target] and count < 10 do
    seen[target] = true
    destroyEntity(target)
    count = count + 1
    target = getCollidedEntity(id, {"box", "ball"})
    destroySelf()
  end
end

return {
  name = "bullet_p",
}
