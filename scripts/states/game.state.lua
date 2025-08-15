state_name = "GameState"
state_category = "STATIC_GAME_STATE"
dimension = "MAPPED"

layer_scripts = {
  "scripts/layers/gameplay/game.layer.lua",
  "scripts/layers/gameplay/hud.layer.lua"
}

function initialize()
  print("Initializing GameState")
  setBackgroundColor(50, 45, 34, 255)
  loadMapAsset("grass_map")
  
  spawnEntity("player", nil, nil, "game")
  spawnEntity("teleporter", nil, nil, "game")
  setPlayerEntity("player", 100, 100)

  local seeder_id = startEntitySeeder(nil, "game", nil, { x = 1280, y = 800 })
  addEntityToSeed("ball", seeder_id, 5)
  addEntityToSeed("box", seeder_id, 5)
  addEntityToSeed("wall_v", seeder_id, 5)
  addEntityToSeed("wall_h", seeder_id, 5)

  spawnEntity("ammo_meter", nil, nil, "hud")
  spawnEntity("speed_meter", nil, nil, "hud")
end

function onEnter() end
function onExit() end
function update() end
function render() end
