#ifndef STATES_KEYS_H
#define STATES_KEYS_H

namespace Project::Libraries::Keys {
  constexpr const char* STATE_INITIALIZE = "initialize";
  constexpr const char* STATE_UPDATE = "update";
  constexpr const char* STATE_RENDER = "render";
  constexpr const char* STATE_ON_ATTACH = "onAttach";
  constexpr const char* STATE_ON_DETACH = "onDetach";
  constexpr const char* STATE_ON_ENTER = "onEnter";
  constexpr const char* STATE_ON_EXIT = "onExit";
  constexpr const char* STATE_HANDLE_INPUT = "handleInput";
  constexpr const char* STATE_PRINT = "print";
}

#endif
