x = center_x + 30
y = center_y - 30
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
  name = "settings_button",
}