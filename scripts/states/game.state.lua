stateName = "GameState"
stateCategory = "STATIC_GAME_STATE"

player = dofile("scripts/entities/player.entity.lua")

function initialize()
  print("Initializing GameState")
  setBackgroundColor(34, 45, 50, 255)
  spawnEntity("player")
end

function onEnter()
  print("Entering GameState")
end

function onExit()
  print("Exit GameState")
end

function update(deltaTime)
  print("Updating GameState")
end

function render()
  print("Rendering GameState")
end
