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
      { key = "left", action = "move_left" },
      { key = "right", action = "move_right" },
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
    rotation_speed = 10.0,
    movement_mode = "VEHICLE"
  }
}

function action_1()
  local vx, vy = getEntityVelocity("player")
  local dx, dy
  if vx and vy and (vx ~= 0 or vy ~= 0) then
    local mag = math.sqrt(vx * vx + vy * vy)
    dx = vx / mag
    dy = vy / mag
  else
    local rad = math.rad(angle)
    dx = math.cos(rad)
    dy = math.sin(rad)
  end
  local spawnDist = 33
  spawn(8 + dx * spawnDist, 8 + dy * spawnDist, dx * 800, dy * 800)
end

function update()
  local dt = deltaTime
  if isActionPressed("player", "move_left") then
    angle = angle - rot_speed * dt
    turnLeft("player", rot_speed)
  elseif isActionPressed("player", "move_right") then
    angle = angle + rot_speed * dt
    turnRight("player", rot_speed)
  end
end

function brake()
  brakeEntity("player")
end

return {
  name = "player"
}
