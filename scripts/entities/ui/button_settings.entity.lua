x = center_x + 110
y = center_y + 40
z = 0


group = "ui"

components = {
  ButtonComponent = {
    component = "ButtonComponent",
    class = "button",
    text = "Settings",
    callbacks = "open_settings"
  }
}

function open_settings()
  changeState("SettingsState")
end

return {
  name = "button_settings",
}
