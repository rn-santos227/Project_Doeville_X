x = 15
y = 15
z = 0

group = "ui"

components = {
  NumericComponent = {
    component = "NumericComponent",
    active = true,
    health = { value = 5, limit = 5 }
  },

  TextComponent = {
    component = "TextComponent",
    class = "meter-label",
    active = true,
    text = "Health"
  },

  MeterComponent = {
    component = "MeterComponent",
    class = "hp-meter",
    active = true,
    width = 160,
    height = 16,
    name = "health",
    orientation = "horizontal",
    x = 70
  }
}

function update(deltaTime)
  local hp = getNumericValue("player", "health")
  if hp then
    setNumericValue("health_meter", "health", hp)
  end
end

return {
  name = "health_meter",
}
