#ifndef PHYSICS_UTILS_H
#define PHYSICS_UTILS_H

#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Utilities {
	class PhysicsUtils {
	public:
		static bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);
		static float calculateDistance(float x1, float y1, float x2, float y2);

		static SDL_FPoint applyGravity(SDL_FPoint velocity, float deltaTime);

	};
}

#endif
