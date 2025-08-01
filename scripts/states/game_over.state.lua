state_name = "GameOverState"
state_category = "GAME_OVER_STATE"
dimension = "BOXED"

function initialize()
  setBackgroundColor(0, 0, 0, 255)
  spawnEntity("game_over_text")
  spawnEntity("button_main_menu")
end

function onEnter()
  print("Entering GameOverState")
end

function onExit()
  print("Exit GameOverState")
end

function update(deltaTime)
  print("Updating GameOverState")
end

function render()
  print("Rendering GameOverState")
end
