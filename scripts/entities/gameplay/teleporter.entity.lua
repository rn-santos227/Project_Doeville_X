x = 900
y = 360
z = 0

group = "gameplay"

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = false,
    boxes = {
      { x = 0, y = 0, w = 64, h = 64 }
    }
  },

  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    radius = 32,
    color_hex = "#00008B"
  },

  PortalComponent = {
    component = "PortalComponent",
    active = true,
    target_x = 2000,
    target_y = 1500,
    trigger_on_touch = true
  }
}

return {
  name = "teleporter"
}