state_name = "SettingsState"
state_category = "SETTINGS_STATE"

function initialize()
  print("Initializing SettingsState")
  setBackgroundColor(50, 50, 60, 255)
end

function onEnter()
  print("Entering SettingsState")
end

function onExit()
  print("Exit SettingsState")
end

function update(deltaTime)
  print("Updating SettingsState")
end

function render()
  print("Rendering SettingsState")
end
