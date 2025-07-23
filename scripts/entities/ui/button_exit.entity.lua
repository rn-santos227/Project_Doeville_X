x = center_x + 30
y = center_y + 10
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
  name = "exit_button",
}
