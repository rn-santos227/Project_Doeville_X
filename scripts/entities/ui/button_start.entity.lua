x = 400
y = 400
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
  name = "start_button",
}
