state_name = "GameState"
state_category = "STATIC_GAME_STATE"
dimension = "BOUNDED"

layer_scripts = {
  "scripts/layers/gameplay/game.layer.lua",
  "scripts/layers/gameplay/hud.layer.lua"
}

function initialize()
  print("Initializing GameState")
  setBackgroundColor(50, 45, 34, 255)
  setMapSize(2560, 1440)
  
  spawnEntity("player", nil, nil, "game")
  setPlayerEntity("player", 100, 100)

  local seeder_id = startEntitySeeder(nil, "game", nil, { x = 1280, y = 720 })
  addEntityToSeed("ball", seeder_id, 5)
  addEntityToSeed("box", seeder_id, 5)
  addEntityToSeed("wall_v", seeder_id, 5)
  addEntityToSeed("wall_h", seeder_id, 5)

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
