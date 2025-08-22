group = "controls"

components = {
  KeysComponent = {
    component = 'KeysComponent',
    active = true,
    keys = {
      { key = 'escape', action = 'escape' }
    },
    bindings = {
      escape = 'open_exit_modal'
    }
  }
}

function open_exit_modal()
  exit_to_menu_requested = true
end

return {
  name = 'game_input',
}
