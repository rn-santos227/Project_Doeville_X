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

function onEnter()
  print("Entering MainMenuState")
end

function onExit()
  print("Exit MainMenuState")
end

function update()
  print("Updating MainMenuState")
end

function render()
  print("Rendering MainMenuState")
end
