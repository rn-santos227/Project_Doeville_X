#ifndef CATEGORIES_H
#define CATEGORIES_H

namespace Project::Libraries::Categories {
  namespace Entities {
    constexpr const char* PLAYER = "player";
    constexpr const char* ENEMY = "enemy";
    constexpr const char* NPC = "npc";
    constexpr const char* PROJECTILE = "projectile";
    constexpr const char* OBSTACLE = "obstacle";
    constexpr const char* VEHICLE = "vehicle";
    constexpr const char* TRIGGER = "trigger";
    constexpr const char* ENVIRONMENT = "environment";
    constexpr const char* PICKUP = "pickup";
    constexpr const char* DOOR = "door";
    constexpr const char* CHECKPOINT = "checkpoint";
    constexpr const char* CAMERA = "camera";
    constexpr const char* UI_ELEMENT = "ui_element";
  }

  namespace States {
    constexpr const char* MAIN_MENU_STATE = "MAIN_MENU_STATE";
    constexpr const char* SETTINGS_STATE = "SETTINGS_STATE";
    constexpr const char* PAUSE_STATE = "PAUSE_STATE";
    constexpr const char* LOADING_STATE = "LOADING_STATE";
    constexpr const char* TRANSITION_STATE = "TRANSITION_STATE";
    constexpr const char* STATIC_GAME_STATE = "STATIC_GAME_STATE";
    constexpr const char* DYNAMIC_GAME_STATE = "DYNAMIC_GAME_STATE";
    constexpr const char* CINEMATIC_STATE = "CINEMATIC_STATE";
    constexpr const char* FPS_GAMEPLAY_STATE = "FPS_GAMEPLAY_STATE";
    constexpr const char* SIDE_SCROLLER_STATE = "SIDE_SCROLLER_STATE";
    constexpr const char* PUZZLE_GAMEPLAY_STATE = "PUZZLE_GAMEPLAY_STATE";
    constexpr const char* VR_GAMEPLAY_STATE = "VR_GAMEPLAY_STATE";
    constexpr const char* BATTLE_STATE = "BATTLE_STATE";
    constexpr const char* DIALOGUE_STATE = "DIALOGUE_STATE";
    constexpr const char* INVENTORY_STATE = "INVENTORY_STATE";
    constexpr const char* MAP_STATE = "MAP_STATE";
    constexpr const char* PUZZLE_STATE = "PUZZLE_STATE";
    constexpr const char* SHOP_STATE = "SHOP_STATE";
    constexpr const char* STORY_STATE = "STORY_STATE";
    constexpr const char* VICTORY_STATE = "VICTORY_STATE";
    constexpr const char* GAME_OVER_STATE = "GAME_OVER_STATE";
    constexpr const char* CREDITS_STATE = "CREDITS_STATE";
    constexpr const char* DEBUG_STATE = "DEBUG_STATE";
    constexpr const char* EDITOR_STATE = "EDITOR_STATE";
  }

  namespace Components {
    constexpr const char* SUFFIX = "component";

    constexpr const char* BOUNDING_BOX = "boundingbox";
    constexpr const char* BUTTON = "button";
    constexpr const char* CURSOR = "cursor";
    constexpr const char* GRAPHICS = "graphics";
    constexpr const char* HUD = "hud";
    constexpr const char* INPUT = "input";
    constexpr const char* KEYS = "keys";
    constexpr const char* MOTION = "motion";
    constexpr const char* PHYSICS = "physics";
    constexpr const char* TEXT = "text";
    constexpr const char* TRANSFORM = "transform";
    constexpr const char* UNKNOWN = "unknown";

    constexpr const char* CAMERA_COMPONENT = "CameraComponent";
    constexpr const char* BOUNDING_BOX_COMPONENT = "BoundingBoxComponent";
    constexpr const char* GRAPHICS_COMPONENT = "GraphicsComponent";
    constexpr const char* KEYS_COMPONENT = "KeysComponent";
    constexpr const char* MOTION_COMPONENT = "MotionComponent";
    constexpr const char* PHYSICS_COMPONENT = "PhysicsComponent";
    constexpr const char* TEXT_COMPONENT = "TextComponent";
  }

  namespace Layers {
    constexpr const char* HUD = "hud";
    constexpr const char* OVERLAY = "overlay";
    constexpr const char* FOREGROUND = "foreground";
    constexpr const char* MIDGROUND = "midground";
    constexpr const char* BACKGROUND = "background";
    constexpr const char* CINEMATIC = "cinematic";
    constexpr const char* CUSTOM = "custom";
  }
}

#endif
