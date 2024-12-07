#include "GameEngine.h"

GameEngine::~GameEngine() {
  std::cout << "Game Engine Shut Down!" << std::endl;
}

void GameEngine::init() {
  std::cout << "Initializing Game Engine..." << std::endl;
  isRunning = true;
  lastTime = std::chrono::steady_clock::now();
}


void GameEngine::update() {
  auto now = std::chrono::steady_clock::now();
  std::chrono::duration<float> deltaTime = now - lastTime;
  lastTime = now;

  frameCount++;
  if (frameCount % 60 == 0) { 
    std::cout << "FPS: " << (1.0f / deltaTime.count()) << std::endl;
  }
}

void GameEngine::render() {
  std::cout << "Rendering game objects..." << std::endl;
}