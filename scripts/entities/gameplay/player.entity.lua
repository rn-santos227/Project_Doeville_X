x = 640
y = 360
z = 0

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
      { key = "left", action = "move_left" },
      { key = "down", action = "move_down" },
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
    rotation_speed = 10.0
  }
}

function action_1()
  local dx = 0
  local dy = 0
  if isActionPressed("player", "move_up") then dy = dy - 1 end
  if isActionPressed("player", "move_down") then dy = dy + 1 end
  if isActionPressed("player", "move_left") then dx = dx - 1 end
  if isActionPressed("player", "move_right") then dx = dx + 1 end

  if dx == 0 and dy == 0 then
    dy = -1
  end

  local mag = math.sqrt(dx * dx + dy * dy)
  dx = dx / mag
  dy = dy / mag

  spawn(dx * 16, dy * 16, dx * 200, dy * 200)
end

function brake()
  brakeEntity("player")
end

return {
  name = "player"
}