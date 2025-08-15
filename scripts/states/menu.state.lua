state_name = "MainMenuState"
state_category = "MAIN_MENU_STATE"
dimension = "BOXED"

function initialize()
  print("Initializing MainMenuState")
  setBackgroundColor(34, 45, 50, 255)
  spawnEntity("sample_text")
  
  spawnEntity("button_start")
  spawnEntity("button_settings")
  spawnEntity("button_exit")

  spawnEntity("menu_input")
end

function onEnter() end
function onExit() end
function update() end
function render() end
