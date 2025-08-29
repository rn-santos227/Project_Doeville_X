state_name = "GameState"
state_category = "STATIC_GAME_STATE"
dimension = "MAPPED"

exit_to_menu_requested = false

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

  spawnEntity("health_meter", nil, nil, "hud")
  spawnEntity("ammo_meter", nil, nil, "hud")
  spawnEntity("speed_meter", nil, nil, "hud")

  spawnEntity("game_input")
  spawnEntity("exit_modal", nil, nil, "hud")
end

function show_exit_modal()
  return exit_to_menu_requested
end

function confirm_exit_to_menu()
  exit_to_menu_requested = false
  changeState("MainMenuState")
end

function cancel_exit_to_menu()
  exit_to_menu_requested = false
end

function onEnter() end
function onExit() end
function update() end
function render() end
