x = 640
y = 360
z = 0

local rot_speed = 10
local max_ammo = 8
local reload_time = 2
local reload_timer = 0
local fire_rate = 0.2
local fire_timer = 0
local max_hp = 5

group = "gameplay"

components = {
  CameraComponent = {
    component = "CameraComponent",
    active = true,
    intensity = 10.0,
    follow_speed = 100.0
  },

  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = true,
    rotation = true,
    boxes = {
      { x = 0, y = 0, w = 32, h = 32 }
    },
    use_proxy = true
  },
  
  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    rotation = true
  },

  NumericComponent = {
    component = "NumericComponent",
    active = true,
    health = { value = max_hp, limit = max_hp },
    ammo = { value = max_ammo, limit = max_ammo }
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
      { key = "x", action = "action_2" },
      { key = "r", action = "reload" }
    },
    bindings = {
      action_1 = "action_1",
      action_2 = "brake",
      reload = "reload",
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
  local ammo = getNumericValue("player", "ammo")
  if ammo and ammo > 0 and reload_timer <= 0 and fire_timer <= 0 then
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
    subtractNumericValue("player", "ammo", 1)
    ammo = ammo - 1
    fire_timer = fire_rate
    if ammo == 0 then
      reload_timer = reload_time
    end
  end
end

function reload()
  local ammo = getNumericValue("player", "ammo")
  if ammo and ammo < max_ammo and reload_timer <= 0 then
    reload_timer = reload_time
    setNumericValue("player", "ammo", 0)
  end
end

function brake()
  brakeEntity("player")
end

function update()
  ignoreCollisionsWith(id, {"bullet_p"})
  if reload_timer > 0 then
    reload_timer = reload_timer - deltaTime
    if reload_timer <= 0 then
      setNumericValue("player", "ammo", max_ammo)
    end
  end

  if fire_timer > 0 then
    fire_timer = fire_timer - deltaTime
    if fire_timer < 0 then fire_timer = 0 end
  end

  if getCollidedEntity(id, {"wall_h", "wall_v"}) then
    cameraShake()
  end
  
  if isActionPressed("player", "move_left") then
    turnLeft("player", rot_speed)
  elseif isActionPressed("player", "move_right") then
    turnRight("player", rot_speed)
  end
end

return {
  name = "player"
}
