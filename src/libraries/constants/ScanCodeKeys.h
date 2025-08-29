#ifndef SCAN_CODE_CONSTANTS_H
#define SCAN_CODE_CONSTANTS_H

#include <SDL.h>

namespace Project::Libraries::Constants {
  constexpr SDL_Scancode KEY_FUNC_HELP = SDL_SCANCODE_F1;
  constexpr SDL_Scancode KEY_FUNC_DEBUG = SDL_SCANCODE_F2;
  constexpr SDL_Scancode KEY_FUNC_FREEZE = SDL_SCANCODE_F3;
  constexpr SDL_Scancode KEY_FUNC_RESTART = SDL_SCANCODE_F4;
  constexpr SDL_Scancode KEY_FUNC_EXIT = SDL_SCANCODE_F5;

  constexpr const char* MOVE_UP = "MOVE_UP";
  constexpr const char* MOVE_DOWN = "MOVE_DOWN";
  constexpr const char* MOVE_LEFT = "MOVE_LEFT";
  constexpr const char* MOVE_RIGHT = "MOVE_RIGHT";
  constexpr const char* ESCAPE = "ESCAPE";
  constexpr const char* ENTER = "ENTER";
  constexpr const char* CONFIRM = "CONFIRM";
  constexpr const char* MOVE_AXIS_X = "MOVE_AXIS_X";
  constexpr const char* MOVE_AXIS_Y = "MOVE_AXIS_Y";
  constexpr const char* ACTION_1 = "ACTION_1";
  constexpr const char* ACTION_2 = "ACTION_2";
  constexpr const char* ACTION_3 = "ACTION_3";
  constexpr const char* ACTION_4 = "ACTION_4";
  constexpr const char* LEFT_ACTION_1 = "LEFT_ACTION_1";
  constexpr const char* LEFT_ACTION_2 = "LEFT_ACTION_2";
  constexpr const char* LEFT_ACTION_3 = "LEFT_ACTION_3";
  constexpr const char* RIGHT_ACTION_1= "RIGHT_ACTION_1";
  constexpr const char* RIGHT_ACTION_2= "RIGHT_ACTION_2";
  constexpr const char* RIGHT_ACTION_3= "RIGHT_ACTION_3";
  constexpr const char* CAMERA_UP = "CAMERA_UP";
  constexpr const char* CAMERA_DOWN = "CAMERA_DOWN";
  constexpr const char* CAMERA_LEFT = "CAMERA_LEFT";
  constexpr const char* CAMERA_RIGHT = "CAMERA_RIGHT";
  constexpr const char* CAMERA_ZOOM_IN = "CAMERA_ZOOM_IN";
  constexpr const char* CAMERA_ZOOM_OUT = "CAMERA_ZOOM_OUT";
  constexpr const char* CAMERA_ROTATE_CCW = "CAMERA_ROTATE_CCW";
  constexpr const char* CAMERA_ROTATE_CW = "CAMERA_ROTATE_CW";
  constexpr const char* ZOOM_IN = "ZOOM_IN";
  constexpr const char* ZOOM_OUT = "ZOOM_OUT";
  constexpr const char* ROTATE_CCW = "ROTATE_CCW";
  constexpr const char* ROTATE_CW = "ROTATE_CW";
  constexpr const char* INTERACT = "INTERACT";
  constexpr const char* INVENTORY_OPEN= "INVENTORY_OPEN";
  constexpr const char* USE_ITEM = "USE_ITEM";
  constexpr const char* ITEM_0 = "ITEM_0";
  constexpr const char* ITEM_1 = "ITEM_1";
  constexpr const char* ITEM_2 = "ITEM_2";
  constexpr const char* ITEM_3 = "ITEM_3";
  constexpr const char* ITEM_4 = "ITEM_4";
  constexpr const char* ITEM_5 = "ITEM_5";
  constexpr const char* ITEM_6 = "ITEM_6";
  constexpr const char* ITEM_7 = "ITEM_7";
  constexpr const char* ITEM_8 = "ITEM_8";
  constexpr const char* ITEM_9 = "ITEM_9";
  constexpr const char* ATTACK_PRIMARY= "ATTACK_PRIMARY";
  constexpr const char* ATTACK_SECONDARY   = "ATTACK_SECONDARY";
  constexpr const char* BLOCK = "BLOCK";
  constexpr const char* DODGE = "DODGE";
  constexpr const char* RELOAD = "RELOAD";
  constexpr const char* SPRINT = "SPRINT";
  constexpr const char* CROUCH = "CROUCH";
  constexpr const char* JUMP = "JUMP";
  constexpr const char* SWITCH_WEAPON = "SWITCH_WEAPON";
  constexpr const char* MENU_OPEN = "MENU_OPEN";
  constexpr const char* MENU_BACK = "MENU_BACK";
  constexpr const char* MAP_TOGGLE = "MAP_TOGGLE";

  constexpr const char* FUNC_HELP = "FUNC_HELP";
  constexpr const char* FUNC_DEBUG = "FUNC_DEBUG";
  constexpr const char* FUNC_FREEZE = "FUNC_FREEZE";
  constexpr const char* FUNC_RESTART = "FUNC_RESTART";
  constexpr const char* FUNC_EXIT = "FUNC_EXIT";

  constexpr const char* WORD_PLUS = "plus";
  constexpr const char* WORD_MINUS = "minus";
  constexpr const char* WORD_EQUALS = "equals";

  constexpr const char SYM_PLUS = '+';
  constexpr const char SYM_MINUS = '=';
  constexpr const char SYM_EQUALS = '-';


}

#endif
