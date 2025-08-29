x = 15
y = 55
z = 0

group = "ui"

components = {
  NumericComponent = {
    component = "NumericComponent",
    active = true,
    speed = { value = 0, limit = 200 }
  },
  
  TextComponent = {
    component = "TextComponent",
    class = "meter-label",
    active = true,
    text = "Speed"
  },

  MeterComponent = {
    component = "MeterComponent",
    class = "meter",
    active = true,
    width = 160,
    height = 16,
    name = "speed",
    orientation = "horizontal",
    x = 70
  }
}

function update(deltaTime)
  local speed = getEntitySpeed("player")
  if speed then
    setNumericValue("speed_meter", "speed", speed)
  end
end

return {
  name = "speed_meter"
}
