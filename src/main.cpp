#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h> 

#include "core/GameEngine.h"
#include "utilities/exception/EngineException.h"

int main(int argc, char* argv[]) { 
  Project::Core::GameEngine engine;
  
  try {
    engine.init();
    engine.run();
  } catch (const Project::Utilities::EngineException& e) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Engine Error", e.what(), nullptr);
  } catch (const std::exception& e) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unexpected Error", e.what(), nullptr);
  }
  
  return 0;
}
