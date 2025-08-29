#ifndef PHYSICS_UTILS_H
#define PHYSICS_UTILS_H

#include <SDL.h>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "libraries/constants/Constants.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Utilities {

  struct Velocity {
    float x = 0.0f;
    float y = 0.0f;

    Velocity() = default;
    Velocity(float vx, float vy) : x(vx), y(vy) {}

    void set(float vx, float vy) {
      x = vx;
      y = vy;
    }

    bool isZero() const {
      return x == 0.0f && y == 0.0f;
    }
  };

	class PhysicsUtils {
	public:
    static bool checkCollision(const SDL_FRect& a, const SDL_FRect& b);
    static bool checkCollision(const Project::Utilities::Circle& a, const Project::Utilities::Circle& b);
    static bool checkCollision(const SDL_FRect& rect, const Project::Utilities::Circle& c);
    static bool checkCollision(const Project::Utilities::OrientedBox& a,const Project::Utilities::OrientedBox& b);
		
    static float calculateDistance(float x1, float y1, float x2, float y2);
		static SDL_FPoint applyGravity(SDL_FPoint velocity, float deltaTime);
    
    static Velocity applyGravity(Velocity velocity, float deltaTime);
    static void clampVelocity(Velocity& velocity, float maxSpeed);

    static void applyForces(Velocity& velocity, Velocity& acceleration, Velocity& force, float mass, float deltaTime);
    static void applyResistance(Velocity& velocity, float friction, float density, bool isKinematic, float deltaTime);
		
    static SDL_FPoint getSnapOffset(const SDL_FRect& moving, const SDL_FRect& other, float dx, float dy);
    static SDL_FPoint getCircleSnapOffset(const Project::Utilities::Circle& moving, const Project::Utilities::Circle& other, float dx, float dy);
    static SDL_FPoint getCircleRectSnapOffset(const Project::Utilities::Circle& moving, const SDL_FRect& other, float dx, float dy);
    static SDL_FPoint getRectCircleSnapOffset(const SDL_FRect& moving, const Project::Utilities::Circle& other, float dx, float dy);
    static SDL_FPoint getOBBSnapOffset(const Project::Utilities::OrientedBox& a, const Project::Utilities::OrientedBox& b, float dx, float dy);
	};
}

#endif
