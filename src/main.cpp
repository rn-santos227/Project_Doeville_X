#include <iostream>

int main() {
  std::cout << "Game Engine Initialized!" << std::endl;

  while(true) {
    std::cout << "Running game loop..." << std::endl;

    //temporary breaks the loop
    break;
  }

  std::cout << "Exiting Game Engine." << std::endl;
  return 0;
}