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
    width = TILE_SIZE,
    height = TILE_SIZE,
    x = x,
    y = y,
    passable = true
  }
end

local index = 1
for row = 0, TILESET_ROWS - 1 do
  for col = 0, TILESET_COLS - 1 do
    add_tile(index, col * TILE_SIZE, row * TILE_SIZE)
    index = index + 1
  end
end

function getAsset(name)
  if not name then
    return tiles
  end
  if type(name) == "number" then
    name = "grass_" .. name
  end
  return tiles[name]
end
