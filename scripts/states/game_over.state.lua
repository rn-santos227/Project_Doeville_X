state_name = "GameOverState"
state_category = "GAME_OVER_STATE"
dimension = "BOXED"

function initialize()
  setBackgroundColor(0, 0, 0, 255)
  spawnEntity("game_over_text")
  spawnEntity("button_main_menu")
end

function onEnter() end
function onExit() end
function update() end
function render() end
