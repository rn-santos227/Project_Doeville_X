#ifndef METER_COMPONENT_H
#define METER_COMPONENT_H

#include "MeterData.h"

#include <string>

#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "interfaces/style_interface/Stylable.h"

namespace Project { namespace Entities { class Entity; } }
namespace Project::Components { class NumericComponent; }

namespace Project::Components {
  class MeterComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Stylable {
  public:
    MeterComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager);
    ~MeterComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;
    void onAttach() override;

  private:
    SDL_Renderer* renderer = nullptr;
    Project::Entities::Entity* owner = nullptr;
    
    MeterData data;
    
    NumericComponent* numericComponent = nullptr;
    std::string numericName;
    std::string targetName;  
  };
}

#endif
