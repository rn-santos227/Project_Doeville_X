local COLS = 40
local ROWS = 25
local TILE_COUNT = 9

local tiles = {
  [1] = "grass_1",
  [2] = "grass_2",
  [3] = "grass_3",
  [4] = "grass_4",
  [5] = "grass_5",
  [6] = "grass_6",
  [7] = "grass_7",
  [8] = "grass_8",
  [9] = "grass_9"
}

math.randomseed(os.time())

local map = {}
for row = 1, ROWS do
  map[row] = {}
  for col = 1, COLS do
    map[row][col] = math.random(TILE_COUNT)
  end
end

local asset = {
  category = "map",
  name = "grass_map",
  tag = "grass",
  tiles = tiles,
  map = map
}

function getAsset()
  return asset
end
