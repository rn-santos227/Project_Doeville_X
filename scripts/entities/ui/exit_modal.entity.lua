x = center_x - 150
y = center_y - 100
z = 0

group = 'ui'

components = {
  ModalComponent = {
    component = 'ModalComponent',
    class = 'modal modal-question',
    active = false,
    title = 'Exit to Menu',
    message = 'Return to the main menu?',
    ok_text = 'Yes',
    cancel_text = 'No',
    condition = 'show_exit_modal',
    ok_callback = 'confirm_exit_to_menu',
    cancel_callback = 'cancel_exit_to_menu',
    type = 'QUESTION'
  }
}

return {
  name = 'exit_modal',
}
