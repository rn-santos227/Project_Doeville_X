#ifndef COMPONENT_CATEGORIES_H
#define COMPONENT_CATEGORIES_H

namespace Project::Libraries::Categories {
  namespace Components {
    constexpr const char* SUFFIX = "component";

    constexpr const char* CAMERA = "camera";
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
    constexpr const char* TRANSFORM_COMPONENT = "TransformComponent";
  }
}

#endif
