#ifndef STATE_ID_H
#define STATE_ID_H

namespace Project::States {
  enum class StateID {
    MainMenuState,
    StaticGameState,
    DynamicGameState,
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
}

#endif