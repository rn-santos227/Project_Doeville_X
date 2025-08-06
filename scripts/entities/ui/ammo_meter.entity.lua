x = 15
y = 35
z = 0

group = "ui"

components = {
  MeterComponent = {
    component = "MeterComponent",
    class = "meter",
    active = true,
    width = 160,
    height = 16,
    name = "ammo",
    orientation = "horizontal",
    target = "player"
  }
}

return {
  name = "bullet_meter",
}
