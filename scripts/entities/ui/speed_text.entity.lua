x = 15
y = 15
z = 0

group = "ui"

components = {
  TextComponent = {
    text = "Sample Text",
    component = "TextComponent",
    class = "text"
    active = true,
  }
}

function update(deltaTime)
  local speed = getEntitySpeed("player")
  if speed then
    setEntityText("speed_text", string.format("Speed: %.2f", speed))
  end
end

return {
  name = "speed_text"
}
