#include "ModalComponent.h"
#include "ModalTypeResolver.h"

#include <sstream>

#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::ColorUtils;
  using Project::Services::Style;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  ModalComponent::ModalComponent(
    SDL_Renderer* renderer,
    Project::Utilities::LogsManager& logsManager,
    Project::Utilities::ConfigReader& configReader,
    Project::Handlers::MouseHandler* mouseHandler)
      : BaseComponent(logsManager), renderer(renderer), configReader(configReader), mouseHandler(mouseHandler) {}

  ModalComponent::~ModalComponent() {
    if (titleTexture) {
      SDL_DestroyTexture(titleTexture);
      titleTexture = nullptr;
    }
    if (subtitleTexture) {
      SDL_DestroyTexture(subtitleTexture);
      subtitleTexture = nullptr;
    }
    if (messageTexture) {
      SDL_DestroyTexture(messageTexture);
      messageTexture = nullptr;
    }
    if (okTextTexture) {
      SDL_DestroyTexture(okTextTexture);
      okTextTexture = nullptr;
    }
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }
}
