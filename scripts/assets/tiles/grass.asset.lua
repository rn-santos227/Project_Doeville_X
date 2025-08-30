local path = "resources/assets/sprites/sprite_001.png"
local TILE_SIZE = 32
local TILESET_ROWS = 3
local TILESET_COLS = 3

local tiles = {}

local function add_tile(index, x, y)
  local name = "grass_" .. index
  tiles[name] = {
    category = "tile",
    name = name,
    path = path,
    tag = "grass",
    width = 32,
    height = 32,
    x = x,
    y = y,
    passable = true
  }
end

local index = 1
for row = 0, 2 do
  for col = 0, 2 do
    add_tile(index, col * 32, row * 32)
    index = index + 1
  end
end

function getAsset(name)
  return tiles[name or "grass_1"]
end
