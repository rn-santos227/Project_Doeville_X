stateName = "MainMenuState"

sampleText = dofile("scripts/entities/sample_text.entity.lua")

function initialize()
  print("Initializing MainMenuState")
  setBackgroundColor(34, 45, 50, 255)
  spawnEntity("sample_text")
end

function onEnter()
  print("Entering MainMenuState")
end

function onExit()
  print("Exit MainMenuState")
end

function update(deltaTime)
  print("Updating MainMenuState")
end

function render()
  print("Rendering MainMenuState")
end
