group = "controls"

components = {
  KeysComponent = {
    component = "KeysComponent",
    active = true,
    keys = {
      { key = "return", action = "enter" },
      { key = "escape", action = "escape" }
    },
    bindings = {
      enter = "enter"
    }
  }
}

function enter()
  changeState("GameState")
end

return {
  name = "menu_input"
}