x = center_x + 110
y = center_y + 90
z = 0

group = "ui"

components = {
  ButtonComponent = {
    component = "ButtonComponent",
    class = "button",
    text = "Exit",
    callbacks = "quit_game"
  }
}

function quit_game()
  exitGame()
end

return {
  name = "button_exit",
}
