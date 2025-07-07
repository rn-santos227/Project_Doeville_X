#ifndef PHYSICS_UTILS_H
#define PHYSICS_UTILS_H

#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Utilities {
	class PhysicsUtils {
	public:
		static bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);

	};
}

#endif
