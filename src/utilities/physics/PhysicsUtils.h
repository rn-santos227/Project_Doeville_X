#ifndef PHYSICS_UTILS_H
#define PHYSICS_UTILS_H

#include <SDL.h>

#include "libraries/constants/Constants.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Utilities {
	class PhysicsUtils {
	public:
    static bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);
    static bool checkCollision(const Project::Utilities::Circle& a, const Project::Utilities::Circle& b);
    static bool checkCollision(const SDL_Rect& rect, const Project::Utilities::Circle& c);
		static float calculateDistance(float x1, float y1, float x2, float y2);

		static SDL_FPoint applyGravity(SDL_FPoint velocity, float deltaTime);
		static void clampVelocity(SDL_FPoint& velocity, float maxSpeed);
		static SDL_FPoint getSnapOffset(const SDL_Rect& moving, const SDL_Rect& other, float dx, float dy);
	};
}

#endif
