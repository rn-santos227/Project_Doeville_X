#ifndef STATE_ID_H
#define STATE_ID_H

namespace Project::States {
  enum class StateID {
   // Core States
    MainMenuState,
    SettingsState,
    PauseState,
    LoadingState,
    TransitionState,

    // Gameplay States
    StaticGameState,
    DynamicGameState,
    CinematicState,
    FPSGameplayState,
    SideScrollerState,
    PuzzleGameplayState,
    VRGameplayState,

    // Specialty States
    BattleState,
    DialogueState,
    InventoryState,
    MapState,
    PuzzleState,
    ShopState,
    StoryState,

    // End Game States
    VictoryState,
    GameOverState,

    // Miscellaneous States
    CreditsState,
    DebugState,
    EditorState,
  };
}

#endif