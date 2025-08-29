#include "TransformComponent.h"

#include "components/PositionableComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/constants/FloatConstants.h"
#include "libraries/keys/Keys.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  
  namespace Components = Project::Libraries::Categories::Components;
  namespace Keys = Project::Libraries::Keys;

  TransformComponent::TransformComponent(LogsManager& logsManager, float flexibility, float spin, bool allowRevert)
    : BaseComponent(logsManager) {
    data.set(flexibility, spin, allowRevert);
  }

  void TransformComponent::update(float deltaTime) {
    if (!owner) return;
    auto* manager = owner->getEntitiesManager();
    auto* myBox = dynamic_cast<BoundingBoxComponent*>(owner->getComponent(Components::BOUNDING_BOX_COMPONENT));
    auto* myPhys = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT));

    bool collideHeavier = false;
    if (manager && myBox && myBox->isSolid()) {
      for (const auto& [id, entity] : manager->getAllEntities()) {
        if (!entity || entity.get() == owner) continue;
        auto* otherBox = dynamic_cast<BoundingBoxComponent*>(entity->getComponent(Components::BOUNDING_BOX_COMPONENT));
        if (!otherBox || !otherBox->isSolid()) continue;
        for (const auto& r1 : myBox->getBoxes()) {
          for (const auto& r2 : otherBox->getBoxes()) {
            if (Project::Utilities::PhysicsUtils::checkCollision(r1, r2)) {
              auto* otherPhys = dynamic_cast<PhysicsComponent*>(entity->getComponent(Components::PHYSICS_COMPONENT));
              float otherMass = otherPhys ? otherPhys->getMass() : Project::Libraries::Constants::DEFAULT_MASS;
              float myMass = myPhys ? myPhys->getMass() : Project::Libraries::Constants::DEFAULT_MASS;
              if (otherMass > myMass) collideHeavier = true;
              break;
            }
          }
          if (collideHeavier) break;
        }
        if (collideHeavier) break;
      }
    }

    if (collideHeavier) {
      transform();
    } else if (data.transformed && data.allowRevert) {
      revert();
    }
  }

  void TransformComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.flexibility = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FLEXIBILITY, Project::Libraries::Constants::DEFAULT_HALF));
    data.spin = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::SPIN, 0.0));
    data.allowRevert = luaStateWrapper.getTableBoolean(tableName, Keys::ALLOW_REVERT, true);
  }

  void TransformComponent::reset() {
    data = TransformData{};
  }

  void TransformComponent::transform() {
    if (data.transformed) return;
    auto* myBox = owner ? dynamic_cast<BoundingBoxComponent*>(owner->getComponent(Components::BOUNDING_BOX_COMPONENT)) : nullptr;
    auto* myPhys = owner ? dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT)) : nullptr;
    auto* gfx = owner ? dynamic_cast<GraphicsComponent*>(owner->getComponent(Components::GRAPHICS_COMPONENT)) : nullptr;
    if (!myBox) return;

    data.originalBoxes = myBox->getBoxes();
    data.originalCircles = myBox->getCircles();

    myBox->clearShapes();
    for (const auto& b : data.originalBoxes) {
      SDL_FRect n;
      n.w = b.w * data.flexibility;
      n.h = b.h * data.flexibility;
      n.x = b.x + (b.w - n.w) * Project::Libraries::Constants::DEFAULT_HALF;
      n.y = b.y + (b.h - n.h) * Project::Libraries::Constants::DEFAULT_HALF;
      myBox->addBox(n);
    }
    for (const auto& c : data.originalCircles) {
      float nr = c.r * data.flexibility;
      myBox->addCircle(c.x, c.y, nr);
    }
    if (myPhys) {
      if (data.spin != 0.0f) {
        myPhys->setAngularVelocity(data.spin);
      }
      myPhys->setRotationEnabled(true);
    }
    if (gfx) {
      gfx->setRotationEnabled(true);
    }
    data.transformed = true;
  }

  void TransformComponent::revert() {
    if (!data.transformed) return;
    auto* myBox = owner ? dynamic_cast<BoundingBoxComponent*>(owner->getComponent(Components::BOUNDING_BOX_COMPONENT)) : nullptr;
    auto* myPhys = owner ? dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT)) : nullptr;
    auto* gfx = owner ? dynamic_cast<GraphicsComponent*>(owner->getComponent(Components::GRAPHICS_COMPONENT)) : nullptr;
    if (!myBox) return;

    myBox->clearShapes();
    for (const auto& b : data.originalBoxes) {
      myBox->addBox(b);
    }
    for (const auto& c : data.originalCircles) {
      myBox->addCircle(c.x, c.y, c.r);
    }
    if (myPhys) {
      myPhys->setAngularVelocity(0.0f);
      myPhys->setRotationEnabled(false);
    }
    if (gfx) {
      gfx->setRotationEnabled(false);
    }
    data.transformed = false;
  }
}
