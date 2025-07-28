x = 640
y = 360
z = 0

local angle = -90
local rot_speed = 10

group = "gameplay"

components = {
  CameraComponent = {
    component = "CameraComponent",
    active = true
  },

  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = true,
    rotation = true,
    boxes = {
      { x = 0, y = 0, w = 32, h = 32 }
    }
  },
  
  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    rotation = true
  },

  SpawnerComponent = {
    component = "SpawnerComponent",
    active = true,
    template = "bullet_p",
    rate = 0.2
  },
  
  KeysComponent = {
    component = "KeysComponent",
    active = true,
    keys = {
      { key = "up", action = "move_up" },
      { key = "down", action = "move_down" },
      { key = "z", action = "action_1" },
      { key = "x", action = "action_2" }
    },
    bindings = {
      action_1 = "action_1",
      action_2 = "brake"
    }
  },

  PhysicsComponent = {
    component = "PhysicsComponent",
    active = true,
    friction = 0.2,
    restitution = 0.5,
    force = 1.0,
    mass = 1.0,
    rotation = true
  },
  
  MotionComponent = {
    component = "MotionComponent",
    active = true,
    brake_power = 0.5,
    speed = 200.0,
    acceleration = 50.0,
    friction = 20.0,
    use_acceleration = true,
    rotation = true,
    rotation_speed = 10.0
  }
}

function action_1()
  local rad = math.rad(angle)
  local dx = math.cos(rad)
  local dy = math.sin(rad)
  local spawnDist = 33
  spawn(8 + dx * spawnDist, 8 + dy * spawnDist, dx * 800, dy * 800)
end

function update()
  local dt = deltaTime
  if isActionPressed("player", "move_left") then
    angle = angle - rot_speed * dt
  elseif isActionPressed("player", "move_right") then
    angle = angle + rot_speed * dt
  end
end

function brake()
  brakeEntity("player")
end

return {
  name = "player"
}
