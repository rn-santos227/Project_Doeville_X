stateName = "GameState"
stateCategory = "STATIC_GAME_STATE"

function initialize()
  print("Initializing GameState")
  setBackgroundColor(34, 45, 50, 255)
  spawnEntity("player")
  
  math.randomseed(os.time())
  for i = 1, 5 do
    local x = math.random(0, 768)
    local y = math.random(0, 568)
    spawnEntity("obstacle", x, y)
  end

  spawnEntity("speed_text")
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
