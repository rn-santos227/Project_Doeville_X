x = 15
y = 15
z = 0

group = "ui"

components = {
  NumericComponent = {
    component = "NumericComponent",
    active = true,
    speed = { value = 0, limit = 200 }
  },
  MeterComponent = {
    component = "MeterComponent",
    class = "meter",
    active = true,
    width = 100,
    height = 10,
    name = "speed",
    orientation = "horizontal"
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
