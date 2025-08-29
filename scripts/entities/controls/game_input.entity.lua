local rot_step = 0.1
group = "controls"

components = {
  KeysComponent = {
    component = 'KeysComponent',
    active = true,
    keys = {
      { key = 'escape', action = 'escape' },
      { key = 'equals', action = 'zoom_in' },
      { key = 'minus', action = 'zoom_out' },
      { key = 'q', action = 'rotate_ccw' },
      { key = 'e', action = 'rotate_cw' }
    },
    bindings = {
      escape = 'open_exit_modal',
      zoom_in = 'zoom_in',
      zoom_out = 'zoom_out',
      rotate_ccw = 'rotate_ccw',
      rotate_cw = 'rotate_cw'
    }
  }
}

function open_exit_modal()
  exit_to_menu_requested = true
end

function zoom_in()
  cameraZoomIn()
end

function zoom_out()
  cameraZoomOut()
end

function rotate_ccw()
  cameraRotate(-rot_step)
end

function rotate_cw()
  cameraRotate(rot_step)
end

return {
  name = 'game_input',
}
