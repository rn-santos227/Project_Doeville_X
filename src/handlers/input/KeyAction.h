#ifndef KEY_ACTION_H
#define KEY_ACTION_H

namespace Project::Handlers {
  enum class KeyAction {
    NONE,

    // **Movement**
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_AXIS_X,
    MOVE_AXIS_Y,

    // **Actions**
    ACTION_1,
    ACTION_2,
    ACTION_3,
    ACTION_4,

    // **Hand Buttons**
    LEFT_ACTION_1,
    LEFT_ACTION_2,
    LEFT_ACTION_3,
    RIGHT_ACTION_1,
    RIGHT_ACTION_2,
    RIGHT_ACTION_3,

    // **Camera Controls**
    CAMERA_UP,
    CAMERA_DOWN,
    CAMERA_LEFT,
    CAMERA_RIGHT,
    CAMERA_ZOOM_IN,
    CAMERA_ZOOM_OUT,
    CAMERA_ROTATE_CCW,
    CAMERA_ROTATE_CW,

    // **Inventory and Interaction**
    INTERACT,
    INVENTORY_OPEN,
    USE_ITEM,
    ITEM_0,
    ITEM_1,
    ITEM_2,
    ITEM_3,
    ITEM_4,
    ITEM_5,
    ITEM_6,
    ITEM_7,
    ITEM_8,
    ITEM_9,

    // **Combat**
    ATTACK_PRIMARY,
    ATTACK_SECONDARY,
    BLOCK,
    DODGE,
    RELOAD,

    // **Other Game-Specific Actions**
    SPRINT,
    CROUCH,
    JUMP,
    SWITCH_WEAPON,

    // **Menu and Navigation**
    MENU_OPEN,
    MENU_BACK,
    ESCAPE,
    ENTER,
    CONFIRM,
    MAP_TOGGLE,

    // **System and Debugging**
    HELP_TOGGLE,
    DEBUG_TOGGLE,
    FREEZE_GAME,
    RESTART_GAME,
    IMMEDIATE_EXIT,
  };
}

#endif
