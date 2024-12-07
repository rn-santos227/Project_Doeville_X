#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h> 
#include "core/GameEngine.h"

int main(int argc, char* argv[]) { 
  GameEngine engine;
  engine.init();
  engine.run();  
  return 0;
}