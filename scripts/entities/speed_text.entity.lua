x = 15
y = 15
z = 0

components = {
  TextComponent = {
    text = "Sample Text",
    component = "TextComponent",
    active = true,
    font_alpha = 255,
    font_color_hex = "FFFFFF",
    font_size = 24
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
