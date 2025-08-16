#ifndef MODAL_COMPONENT_H
#define MODAL_COMPONENT_H

#include "ModalData.h"
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

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::MODAL; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;

    void setEntityPosition(float x, float y) override;
    void setSize(int w, int h);

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

  private:
    SDL_Renderer* renderer = nullptr;
    Project::Utilities::ConfigReader& configReader;

    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::MouseHandler* mouseHandler = nullptr;
    
    ModalData data;

    void createTitleTexture();
    void createSubtitleTexture();
    void createMessageTexture();
    void createOkTextTexture();
    void createCancelTextTexture();
    void positionOkButton();
    void positionQuestionButtons();
  };
}

#endif
