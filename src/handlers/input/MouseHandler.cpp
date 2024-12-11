#include "MouseHandler.h"

MouseHandler::MouseHandler(LogsManager& logsManager) : mouseX(0), mouseY(0), mouseWheelScroll(0), logsManager(logsManager) {}
MouseHandler::~MouseHandler() {}