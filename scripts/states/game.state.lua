state_name = "GameState"
state_category = "STATIC_GAME_STATE"

layer_scripts = {
  "scripts/layers/gameplay/game.layer.lua",
  "scripts/layers/gameplay/hud.layer.lua"
}

function initialize()
  print("Initializing GameState")
  setBackgroundColor(50, 45, 34, 255)
  spawnEntity("player", nil, nil, "game")
  setActiveCamera("player")

  startEntitySeeder()
  spawnEntity("speed_text", nil, nil, "hud")
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
