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

  math.randomseed(os.time())
  for i = 1, 5 do
    local x = math.random(0, 768)
    local y = math.random(0, 568)
    spawnEntity("obstacle", x, y, "game")
  end

  for i = 1, 8 do
    local x = math.random(0, 768)
    local y = math.random(0, 568)
    spawnEntity("ball", x, y, "game")
  end

  for i = 1, 8 do
    local x = math.random(0, 1000)
    local y = math.random(0, 600)
    spawnEntity("wall_v", x, y, "game")
  end

  for i = 1, 8 do
    local x = math.random(0, 1000)
    local y = math.random(0, 600)
    spawnEntity("wall_h", x, y, "game")
  end

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
