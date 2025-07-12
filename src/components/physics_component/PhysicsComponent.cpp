#include "PhysicsComponent.h"

#include "components/PositionableComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Components {
  PhysicsComponent::PhysicsComponent(Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager),
     weight(Project::Libraries::Constants::DEFAULT_WEIGHT),
     density(Project::Libraries::Constants::DEFAULT_DENSITY) {}

  void PhysicsComponent::update(float deltaTime) {
    velocityX += accelerationX * deltaTime;
    velocityY += accelerationY * deltaTime;
    accelerationX = 0.0f;
    accelerationY = 0.0f;

    if (friction > 0.0f) {
      float decel = friction * deltaTime;
      if (velocityX > 0.0f) {
        velocityX -= decel;
        if (velocityX < 0.0f) velocityX = 0.0f;
      } else if (velocityX < 0.0f) {
        velocityX += decel;
        if (velocityX > 0.0f) velocityX = 0.0f;
      }
      if (velocityY > 0.0f) {
        velocityY -= decel;
        if (velocityY < 0.0f) velocityY = 0.0f;
      } else if (velocityY < 0.0f) {
        velocityY += decel;
        if (velocityY > 0.0f) velocityY = 0.0f;
      }
    }

    if (density > 0.0f) {
      float factor = Constants::DEFAULT_WHOLE - density * deltaTime;
      if (factor < 0.0f) factor = 0.0f;
      velocityX *= factor;
      velocityY *= factor;
    }

    if (owner) {
      float oldX = owner->getX();
      float oldY = owner->getY();
      float newX = oldX + velocityX * deltaTime;
      float newY = oldY + velocityY * deltaTime;
      owner->setPosition(newX, newY);

      for (const std::string& name : owner->listComponentNames()) {
        if (auto* comp = owner->getComponent(name)) {
          if (auto* pos = dynamic_cast<PositionableComponent*>(comp)) {
            pos->setEntityPosition(static_cast<int>(newX), static_cast<int>(newY));
          }
        }
      }

      auto* manager = owner->getEntitiesManager();
      auto* myBox = dynamic_cast<BoundingBoxComponent*>(owner->getComponent("BoundingBoxComponent"));
      if (manager && myBox && myBox->isSolid()) {
        for (const auto& [id, entity] : manager->getAllEntities()) {
          if (!entity || entity.get() == owner) continue;
          auto* otherBox = dynamic_cast<BoundingBoxComponent*>(entity->getComponent("BoundingBoxComponent"));
          if (!otherBox || !otherBox->isSolid()) continue;
          
          for (const auto& r1 : myBox->getBoxes()) {
            for (const auto& r2 : otherBox->getBoxes()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(r1, r2)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;

                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent("PhysicsComponent"));
                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getSnapOffset(r1, r2, velocityX * deltaTime, velocityY * deltaTime);

                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                owner->setPosition(snapX, snapY);
                for (const std::string& n : owner->listComponentNames()) {
                  if (auto* c = owner->getComponent(n)) {
                    if (auto* pos = dynamic_cast<PositionableComponent*>(c)) {
                      pos->setEntityPosition(static_cast<int>(snapX), static_cast<int>(snapY));
                    }
                  }
                }

                velocityX = -velocityX * bounce;
                velocityY = -velocityY * bounce;
                velocityX *= (Constants::DEFAULT_WHOLE - fric);
                velocityY *= (Constants::DEFAULT_WHOLE - fric);
                return;
              }
            }

            for (const auto& c2 : otherBox->getCircles()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(r1, c2)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;

                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent("PhysicsComponent"));
                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getRectCircleSnapOffset(r1, c2, velocityX * deltaTime, velocityY * deltaTime);

                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                owner->setPosition(snapX, snapY);
                for (const std::string& n : owner->listComponentNames()) {
                  if (auto* c = owner->getComponent(n)) {
                    if (auto* pos = dynamic_cast<PositionableComponent*>(c)) {
                      pos->setEntityPosition(static_cast<int>(snapX), static_cast<int>(snapY));
                    }
                  }
                }

                velocityX = -velocityX * bounce;
                velocityY = -velocityY * bounce;
                velocityX *= (Constants::DEFAULT_WHOLE - fric);
                velocityY *= (Constants::DEFAULT_WHOLE - fric);
                return;
              }
            }
          }

          for (const auto& c1 : myBox->getCircles()) {
            for (const auto& c2 : otherBox->getCircles()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(c1, c2)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;

                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent("PhysicsComponent"));
                if (otherPhysics && pushForce > 0.0f) {
                  float pushX = velocityX * pushForce;
                  float pushY = velocityY * pushForce;
                  otherPhysics->addVelocity(pushX, pushY);
                }

                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getCircleSnapOffset(c1, c2, velocityX * deltaTime, velocityY * deltaTime);
                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                owner->setPosition(snapX, snapY);
                for (const std::string& n : owner->listComponentNames()) {
                  if (auto* c = owner->getComponent(n)) {
                    if (auto* pos = dynamic_cast<PositionableComponent*>(c)) {
                      pos->setEntityPosition(static_cast<int>(snapX), static_cast<int>(snapY));
                    }
                  }
                }

                velocityX = -velocityX * bounce;
                velocityY = -velocityY * bounce;
                velocityX *= (Constants::DEFAULT_WHOLE - fric);
                velocityY *= (Constants::DEFAULT_WHOLE - fric);
                return;
              }
            }

            for (const auto& r2 : otherBox->getBoxes()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(r2, c1)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;
                
                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent("PhysicsComponent"));
                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getCircleRectSnapOffset(c1, r2, velocityX * deltaTime, velocityY * deltaTime);

                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                owner->setPosition(snapX, snapY);
                for (const std::string& n : owner->listComponentNames()) {
                  if (auto* c = owner->getComponent(n)) {
                    if (auto* pos = dynamic_cast<PositionableComponent*>(c)) {
                      pos->setEntityPosition(static_cast<int>(snapX), static_cast<int>(snapY));
                    }
                  }
                }

                velocityX = -velocityX * bounce;
                velocityY = -velocityY * bounce;
                velocityX *= (Constants::DEFAULT_WHOLE - fric);
                velocityY *= (Constants::DEFAULT_WHOLE - fric);
                return;
              }
            }
          } 
        }
      }
    }
  }
}
