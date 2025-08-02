#ifndef MODAL_COMPONENT_H
#define MODAL_COMPONENT_H

#include "ModalType.h"

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "interfaces/style_interface/Stylable.h"
#include "handlers/input/MouseHandler.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class ModalComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Stylable {
  public:
    ModalComponent(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Utilities::ConfigReader& configReader,
      Project::Handlers::MouseHandler* mouseHandler
    );
    ~ModalComponent() override;

  private:
    SDL_Renderer* renderer = nullptr;
    Project::Utilities::ConfigReader& configReader;

    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::MouseHandler* mouseHandler = nullptr;

    SDL_Rect rect{0,0,0,0};
    SDL_Color color{255,255,255,255};
    SDL_Color borderColor{0,0,0,0};
    int borderWidth{0};

    TTF_Font* font = nullptr;
    SDL_Color fontColor{255,255,255,255};
    SDL_Texture* titleTexture = nullptr;
    SDL_Rect titleRect{0,0,0,0};
    int fontSize = 16;
    std::string title;

    SDL_Texture* subtitleTexture = nullptr;
    SDL_Rect subtitleRect{0,0,0,0};
    std::string subtitle;

    SDL_Texture* messageTexture = nullptr;
    SDL_Rect messageRect{0,0,0,0};
    std::string message;

    SDL_Rect okRect{0,0,80,30};
    SDL_Color okColor{200,200,200,255};
    SDL_Color okBorderColor{0,0,0,0};
    int okBorderWidth{0};
    SDL_Color okTextColor{0,0,0,255};
    SDL_Texture* okTextTexture = nullptr;
    SDL_Rect okTextRect{0,0,0,0};
    std::string okText{"OK"};
    bool okWasPressed{false};
    bool dismissed{false};

    std::string conditionFunc;
    std::string okCallback;
    ModalType modalType{ModalType::NOTIFICATION};

    void createTitleTexture();
    void createSubtitleTexture();
    void createMessageTexture();
    void createOkTextTexture();
    void positionOkButton();
  };
}

#endif
