x = 640
y = 360
z = 0

components = {
  BoundingBoxComponent = {
    component = "BoundingBoxComponent",
    active = true,
    solid = false,
    boxes = {
      { x = 0, y = 0, w = 32, h = 32 }
    }
  },
  
  GraphicsComponent = {
    component = "GraphicsComponent",
    active = true,
    width = 32,
    height = 32,
    colorHex = "ADD8E6",
    colorA = 255
  }
}

return {
  name = "player"
}