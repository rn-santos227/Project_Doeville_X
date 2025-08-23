x = 15
y = 35
z = 0

group = "ui"

components = {
  NumericComponent = {
    component = "NumericComponent",
    active = true,
    ammo = { value = 8, limit = 8 }
  },

  TextComponent = {
    component = "TextComponent",
    class = "meter-label",
    active = true,
    text = "Ammo"
  },

  MeterComponent = {
    component = "MeterComponent",
    class = "ammo-meter",
    active = true,
    width = 160,
    height = 16,
    name = "ammo",
    orientation = "horizontal",
    x = 70
  }
}

function update(deltaTime)
  local ammo = getNumericValue("player", "ammo")
  if ammo then
    setNumericValue("ammo_meter", "ammo", ammo)
  end
end

return {
  name = "ammo_meter",
}
