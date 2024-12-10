#ifndef STATEID_H
#define STATEID_H

enum class StateID {
  StaticGameState,
  DynamicGameState,
  MenuState,
  CinematicState,
  SettingsState,
  PauseState,
  VictoryState,
  GameOverState,
  LoadingState,
  InventoryState,
  MapState,
  PuzzleState,    
  BattleState,
  ShopState,
  DialogueState,
  StoryState,
  FPSGameplayState,
  SideScrollerState,
  PuzzleGameplayState,
  VRGameplayState,
  MainMenuState,
  CreditsState,
  EditorState,
  TransitionState
};

#endif