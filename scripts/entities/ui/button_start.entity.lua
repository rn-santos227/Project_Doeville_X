x = 400
y = 400
z = 0

group = "ui"

components = {
  ButtonComponent = {
    component = "ButtonComponent",
    width = 120,
    height = 40,
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
