x = center_x + 110
y = center_y - 10
z = 0

group = "ui"

components = {
  ButtonComponent = {
    component = "ButtonComponent",
    class = "button",
    text = "Start Game",
    callbacks = "start_game"
  }
}

function start_game()
  changeState("GameState")
end

return {
  name = "button_start",
}
