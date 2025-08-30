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
      { x = 16, y = 16, radius = 16 }
    },
    friction = 0.2,
    rotation = true,
    restitution = 0.0,
    use_proxy = true
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    rotation = true,
    radius = 16
  },

  NumericComponent = {
    component = "NumericComponent",
    active = true,
    hp = { value = 3, limit = 3 },
    flash = { value = 0, limit = 0 }
  },

  MeterComponent = {
    component = "MeterComponent",
    class = "obs-hp-meter",
    active = false,
    name = "hp",
    orientation = "horizontal",
    y = -10
  },

  TimerComponent = {
    component = "TimerComponent",
    active = false,
    seconds = 3,
    callbacks = "hideMeter",
  },

  MotionComponent = {
    component = "MotionComponent",
    active = true,
    use_acceleration = true,
    rotation = true,
    speed = 100.0,
    acceleration = 50.0,
    friction = 20.0
  },

  PhysicsComponent = {
    component = "PhysicsComponent",
    active = true,
    rotation = true,
    static = false,
    kinematic = true,
    gravity = false,
    density = 0.2,
    friction = 0.2,
    mass = 1.0,
    gravity_scale = 1.0,
    restitution = 0.0
  }
}

function hideMeter()
  setMeterActive(id, false)
end

function update(deltaTime)
  local flash = getNumericValue(id, "flash")
  if flash and flash > 0 then
    setColor(id, 255, 255, 255)
    setNumericValue(id, "flash", flash - 1)
  else
    setColor(id, 0, 255, 0)
  end
end

return {
  name = "ball"
}
