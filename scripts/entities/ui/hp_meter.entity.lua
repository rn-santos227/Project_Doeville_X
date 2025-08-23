x = 15
y = 35
z = 0

group = "ui"

components = {
  NumericComponent = {
    component = "NumericComponent",
    active = true,
    hp = { value = 3, limit = 3 }
  },

  TextComponent = {
    component = "TextComponent",
    class = "meter-label",
    active = true,
    text = "HP"
  },

  MeterComponent = {
    component = "MeterComponent",
    class = "hp-meter",
    active = true,
    width = 160,
    height = 16,
    name = "hp",
    orientation = "horizontal",
    x = 60
  }
}

function update(deltaTime)
  local hp = getNumericValue("player", "hp")
  if hp then
    setNumericValue("hp_meter", "hp", hp)
  end
end

return {
  name = "hp_meter",
}
