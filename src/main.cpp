#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h> 
#include "core/GameEngine.h"

int main(int argc, char* argv[]) { 
  std::cout << "Starting main()" << std::endl;

  Project::Core::GameEngine engine; engine;
  
  engine.init();  
  engine.run();  
  
  return 0;
}