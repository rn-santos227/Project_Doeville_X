x = 640
y = 360
z = 0

local rot_speed = 10

group = "gameplay"

components = {
  CameraComponent = {
    component = "CameraComponent",
    active = true,
    intensity = 5.0,
    follow_speed = 10.0,
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
    movement_mode = "vehicle"
  }
}

function action_1()
  local vx, vy = getEntityVelocity("player")
  local dx, dy
  local facing
  if vx and vy and (vx ~= 0 or vy ~= 0) then
    local mag = math.sqrt(vx * vx + vy * vy)
    dx = vx / mag
    dy = vy / mag
    facing = math.deg(math.atan2(dy, dx))
  else
    facing = getEntityRotation("player") or 0
    local rad = math.rad(facing)
    dx = math.cos(rad)
    dy = math.sin(rad)
  end

  local spawnDist = 33
  spawn(8 + dx * spawnDist, 8 + dy * spawnDist, dx * 800, dy * 800, facing)
end

function update()
  ignoreCollisionsWith(id, {"bullet_p"})
  if getCollidedEntity(id, {"wall_h", "wall_v"}) then
    cameraShake()
  end
  if isActionPressed("player", "move_left") then
    turnLeft("player", rot_speed)
  elseif isActionPressed("player", "move_right") then
    turnRight("player", rot_speed)
  end
end

function brake()
  brakeEntity("player")
end

return {
  name = "player"
}
