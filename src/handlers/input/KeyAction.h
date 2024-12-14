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

    // **Menu and Navigation**
    MENU_OPEN,
    MENU_BACK,
    ESCAPE,
    ENTER,
    CONFIRM,

    // **System and Debugging**
    HELP_TOGGLE,
    DEBUG_TOGGLE,
    FREEZE_GAME,
    RESTART_GAME,
    IMMEDIATE_EXIT,
  };
}

#endif