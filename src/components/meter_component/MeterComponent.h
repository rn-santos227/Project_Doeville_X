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
    ComponentType getType() const override { return ComponentType::METER; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;
    void onAttach() override;
    
    void setEntityPosition(float x, float y) override;

    void setSize(int w, int h);
    void setNumericComponent(NumericComponent* comp, const std::string& name);
    void setBarColor(SDL_Color color) { data.barColor = color; }
    void setBackgroundColor(SDL_Color color) { data.backgroundColor = color; }
    void setBorderColor(SDL_Color color) { data.borderColor = color; }
    void setBorderWidth(int width) { data.borderWidth = width; }
    void setCornerRadius(int radius) { data.cornerRadius = radius; }
    void setOrientation(MeterOrientation orient) { data.orientation = orient; }
    void setRound(bool round) { data.isRound = round; }
    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
  
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
