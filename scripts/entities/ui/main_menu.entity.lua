x = center_x + 110
y = center_y + 40
z = 0

group = "ui"

components = {
  ButtonComponent = {
    component = "ButtonComponent",
    class = "button",
    text = "Main Menu",
    callbacks = "goto_menu"
  }
}

function goto_menu()
  changeState("MainMenuState")
end

return {
  name = "button_main_menu"
}
