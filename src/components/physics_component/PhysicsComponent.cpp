#include "PhysicsComponent.h"

#include <cmath>
#include <string>

#include "components/PositionableComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/physics_component/SurfaceTypeResolver.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "interfaces/rotation_interface/Rotatable.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/physics/PhysicsUtils.h"
#include "utilities/math/MathUtils.h"

namespace Project::Components {
  using Project::Utilities::MathUtils;
  using Project::Utilities::PhysicsUtils;
  using Project::Components::Physics::SurfaceType;
  using Project::Components::Physics::SurfaceTypeResolver;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  PhysicsComponent::PhysicsComponent(Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager),
      mass(Project::Libraries::Constants::DEFAULT_MASS),
      density(Project::Libraries::Constants::DEFAULT_DENSITY),
      gravityScale(Project::Libraries::Constants::DEFAULT_GRAVITY_SCALE) {}

  void PhysicsComponent::resolveCollisionWith(PhysicsComponent* other, float restitution) {
    if (!other || !owner || !other->owner) return;

    float nx = other->owner->getX() - owner->getX();
    float ny = other->owner->getY() - owner->getY();
    float dist = MathUtils::magnitude(nx, ny);
    if (dist == 0.0f) {
      if (std::abs(velocityX) > std::abs(velocityY)) {
        nx = (velocityX > 0.0f) ? Constants::DEFAULT_WHOLE : -Constants::DEFAULT_WHOLE;
        ny = 0.0f;
      } else {
        ny = (velocityY > 0.0f) ? Constants::DEFAULT_WHOLE : -Constants::DEFAULT_WHOLE;
        nx = 0.0f;
      }
      dist = Constants::DEFAULT_WHOLE;
    }

    SDL_FPoint norm = MathUtils::normalize(nx, ny);
    nx = norm.x;
    ny = norm.y;

    float relVelX = other->velocityX - velocityX;
    float relVelY = other->velocityY - velocityY;
    float velAlongNormal = MathUtils::dot(relVelX, relVelY, nx, ny);
    if (velAlongNormal > 0.0f) return;

    float invMass1 = (mass > 0.0f) ? Constants::DEFAULT_WHOLE / mass : 0.0f;
    float invMass2 = (other->mass > 0.0f) ? Constants::DEFAULT_WHOLE / other->mass : 0.0f;

    float j = -(Constants::DEFAULT_WHOLE + restitution) * velAlongNormal;
    j /= (invMass1 + invMass2);

    float impulseX = j * nx;
    float impulseY = j * ny;

    velocityX -= impulseX * invMass1;
    velocityY -= impulseY * invMass1;
    other->velocityX += impulseX * invMass2;
    other->velocityY += impulseY * invMass2;

    if (rotationEnabled || other->rotationEnabled) {
      float torque = MathUtils::cross(impulseX, impulseY, nx, ny);
      if (rotationEnabled) angularVelocity -= torque * invMass1;
      if (other->rotationEnabled) other->angularVelocity += torque * invMass2;
    }

    SDL_FPoint vel1{velocityX, velocityY};
    SDL_FPoint vel2{other->velocityX, other->velocityY};
    PhysicsUtils::clampVelocity(vel1, Constants::TERMINAL_VELOCITY);
    PhysicsUtils::clampVelocity(vel2, Constants::TERMINAL_VELOCITY);

    velocityX = vel1.x;
    velocityY = vel1.y;
    other->velocityX = vel2.x;
    other->velocityY = vel2.y;
  }

  void PhysicsComponent::update(float deltaTime) {
    if (isStatic) {
      forceX = forceY = 0.0f;
      accelerationX = accelerationY = 0.0f;
      return;
    }

    if (!isKinematic && gravityEnabled) {
      applyForce(
        Constants::DEFAULT_GRAVITY_DIRECTION.x * getWeight() * gravityScale,
        Constants::DEFAULT_GRAVITY_DIRECTION.y * getWeight() * gravityScale
      );
    }

    PhysicsUtils::applyForces(
      velocityX, velocityY,
      accelerationX, accelerationY,
      forceX, forceY,
      mass, deltaTime
    );
    bool collisionOccurred = false;

    PhysicsUtils::applyResistance(
      velocityX, velocityY,
      friction, density,
      isKinematic, deltaTime
    );

    SDL_FPoint vel{velocityX, velocityY};
    Project::Utilities::PhysicsUtils::clampVelocity(
    vel, Project::Libraries::Constants::TERMINAL_VELOCITY);
    velocityX = vel.x;
    velocityY = vel.y;

    if (owner) {
      float oldX = owner->getX();
      float oldY = owner->getY();
      float newX = oldX + velocityX * deltaTime;
      float newY = oldY + velocityY * deltaTime;
      syncPositionWithComponents(newX, newY);

      auto* manager = owner->getEntitiesManager();
      auto* myBox = dynamic_cast<BoundingBoxComponent*>(owner->getComponent(Components::BOUNDING_BOX_COMPONENT));
      bool exitLoops = false;
      if (manager && myBox && myBox->isSolid()) {
        for (const auto& [id, entity] : manager->getAllEntities()) {
          if (exitLoops) break;
          if (!entity || entity.get() == owner) continue;
          auto* otherBox = dynamic_cast<BoundingBoxComponent*>(entity->getComponent(Components::BOUNDING_BOX_COMPONENT));
          if (!otherBox || !otherBox->isSolid()) continue;
          
          const auto& myRects = myBox->getBoxes();
          const auto& otherRects = otherBox->getBoxes();
          const auto& myOBB = myBox->getOrientedBoxes();
          const auto& otherOBB = otherBox->getOrientedBoxes();
          for (size_t i = 0; i < myRects.size(); ++i) {
            const SDL_Rect& r1 = myRects[i];
            for (size_t j = 0; j < otherRects.size(); ++j) {
              const SDL_Rect& r2 = otherRects[j];
              if (exitLoops) break;
              
              bool collides = false;
              if (myBox->isRotationEnabled() || otherBox->isRotationEnabled()) {
                collides = Project::Utilities::PhysicsUtils::checkCollision(myOBB[i], otherOBB[j]);
              } else {
                collides = Project::Utilities::PhysicsUtils::checkCollision(r1, r2);
              }

              if (collides) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;

                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent(Components::PHYSICS_COMPONENT));
                if (otherPhysics && pushForce > 0.0f && !otherPhysics->getStatic()) {
                  float pushX = velocityX * pushForce;
                  float pushY = velocityY * pushForce;
                  otherPhysics->addVelocity(pushX, pushY);
                }
                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getSnapOffset(r1, r2, velocityX * deltaTime, velocityY * deltaTime);

                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                syncPositionWithComponents(snapX, snapY);
                
                if (otherPhysics) {
                  if (otherPhysics->getStatic()) {
                    auto surface = otherPhysics->getSurfaceType();
                    if (surface == SurfaceType::SLIDE) {
                      if (std::abs(offset.x) >= std::abs(offset.y)) velocityX = 0.0f; else velocityY = 0.0f;
                      velocityX *= (Constants::DEFAULT_WHOLE - fric);
                      velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    } else if (surface == SurfaceType::STICK) {
                      velocityX = 0.0f;
                      velocityY = 0.0f;
                    } else {
                      velocityX = -velocityX * bounce;
                      velocityY = -velocityY * bounce;
                      velocityX *= (Constants::DEFAULT_WHOLE - fric);
                      velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    }
                  } else {
                    resolveCollisionWith(otherPhysics, bounce);
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    otherPhysics->setVelocity(
                      otherPhysics->getVelocityX() * (Constants::DEFAULT_WHOLE - fric),
                      otherPhysics->getVelocityY() * (Constants::DEFAULT_WHOLE - fric));
                  }
                } else {
                  auto surface = otherBox->getSurfaceType();
                  if (surface == SurfaceType::SLIDE) {
                    if (std::abs(offset.x) >= std::abs(offset.y)) velocityX = 0.0f; else velocityY = 0.0f;
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                  } else if (surface == SurfaceType::STICK) {
                    velocityX = 0.0f;
                    velocityY = 0.0f;
                  } else {
                    velocityX = -velocityX * bounce;
                    velocityY = -velocityY * bounce;
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                  }
                }
                collisionOccurred = true;
                break;
              }
            }

            for (const auto& c2 : otherBox->getCircles()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(r1, c2)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;

                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent(Components::PHYSICS_COMPONENT));
                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getRectCircleSnapOffset(r1, c2, velocityX * deltaTime, velocityY * deltaTime);

                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                syncPositionWithComponents(snapX, snapY);

                if (otherPhysics) {

                } else {
                  velocityX = -velocityX * bounce;
                  velocityY = -velocityY * bounce;
                  velocityX *= (Constants::DEFAULT_WHOLE - fric);
                  velocityY *= (Constants::DEFAULT_WHOLE - fric);
                }
                collisionOccurred = true;
                break;
              }
            }
          }

          for (const auto& c1 : myBox->getCircles()) {
            for (const auto& c2 : otherBox->getCircles()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(c1, c2)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;

                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent(Components::PHYSICS_COMPONENT));
                if (otherPhysics && pushForce > 0.0f) {
                  float pushX = velocityX * pushForce;
                  float pushY = velocityY * pushForce;
                  otherPhysics->addVelocity(pushX, pushY);
                }

                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getCircleSnapOffset(c1, c2, velocityX * deltaTime, velocityY * deltaTime);
                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                syncPositionWithComponents(snapX, snapY);

                if (otherPhysics) {
                  if (otherPhysics->getStatic()) {
                    auto surface = otherPhysics->getSurfaceType();
                    if (surface == SurfaceType::SLIDE) {
                      if (std::abs(offset.x) >= std::abs(offset.y)) velocityX = 0.0f; else velocityY = 0.0f;
                      velocityX *= (Constants::DEFAULT_WHOLE - fric);
                      velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    } else if (surface == SurfaceType::STICK) {
                      velocityX = 0.0f;
                      velocityY = 0.0f;
                    } else {
                      velocityX = -velocityX * bounce;
                      velocityY = -velocityY * bounce;
                      velocityX *= (Constants::DEFAULT_WHOLE - fric);
                      velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    }
                  } else {
                    resolveCollisionWith(otherPhysics, bounce);
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    otherPhysics->setVelocity(
                      otherPhysics->getVelocityX() * (Constants::DEFAULT_WHOLE - fric),
                      otherPhysics->getVelocityY() * (Constants::DEFAULT_WHOLE - fric));
                  }
                } else {
                  auto surface = otherBox->getSurfaceType();
                  if (surface == SurfaceType::SLIDE) {
                    if (std::abs(offset.x) >= std::abs(offset.y)) velocityX = 0.0f; else velocityY = 0.0f;
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                  } else if (surface == SurfaceType::STICK) {
                    velocityX = 0.0f;
                    velocityY = 0.0f;
                  } else {
                    velocityX = -velocityX * bounce;
                    velocityY = -velocityY * bounce;
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                  }
                }
                collisionOccurred = true;
                break;
              }
            }

            for (const auto& r2 : otherBox->getBoxes()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(r2, c1)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;
                
                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent(Components::PHYSICS_COMPONENT));
                SDL_FPoint offset = Project::Utilities::PhysicsUtils::getCircleRectSnapOffset(c1, r2, velocityX * deltaTime, velocityY * deltaTime);

                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                syncPositionWithComponents(snapX, snapY);

                if (otherPhysics) {
                  if (otherPhysics->getStatic()) {
                    auto surface = otherPhysics->getSurfaceType();
                    if (surface == SurfaceType::SLIDE) {
                      if (std::abs(offset.x) >= std::abs(offset.y)) velocityX = 0.0f; else velocityY = 0.0f;
                      velocityX *= (Constants::DEFAULT_WHOLE - fric);
                      velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    } else if (surface == SurfaceType::STICK) {
                      velocityX = 0.0f;
                      velocityY = 0.0f;
                    } else {
                      velocityX = -velocityX * bounce;
                      velocityY = -velocityY * bounce;
                      velocityX *= (Constants::DEFAULT_WHOLE - fric);
                      velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    }
                  } else {
                    resolveCollisionWith(otherPhysics, bounce);
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                    otherPhysics->setVelocity(
                      otherPhysics->getVelocityX() * (Constants::DEFAULT_WHOLE - fric),
                      otherPhysics->getVelocityY() * (Constants::DEFAULT_WHOLE - fric)
                    );
                  }
                } else {
                  auto surface = otherBox->getSurfaceType();
                  if (surface == SurfaceType::SLIDE) {
                    if (std::abs(offset.x) >= std::abs(offset.y)) velocityX = 0.0f; else velocityY = 0.0f;
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                  } else if (surface == SurfaceType::STICK) {
                    velocityX = 0.0f;
                    velocityY = 0.0f;
                  } else {
                    velocityX = -velocityX * bounce;
                    velocityY = -velocityY * bounce;
                    velocityX *= (Constants::DEFAULT_WHOLE - fric);
                    velocityY *= (Constants::DEFAULT_WHOLE - fric);
                  }
                }
                collisionOccurred = true;
                break;
              }
            }
          } 
        }
      }
    }

    if (rotationEnabled) {
      updateRotationState(deltaTime, collisionOccurred);
    }
  }

  void PhysicsComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    setActive(active);

    float force = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FORCE, Constants::DEFAULT_PUSH_FORCE));
    setPushForce(force);

    float fric = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FRICTION, Constants::DEFAULT_FRICTION));
    setFriction(fric);

    float mass = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::MASS, Project::Libraries::Constants::DEFAULT_MASS));
    setMass(mass);

    float rest = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::RESTITUTION, Constants::DEFAULT_BOUNCE_FACTOR));
    setRestitution(rest);
    
    float rotSpeed = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::ROTATION_SPEED, Constants::DEFAULT_ROTATION_SPEED));
    setRotationSpeed(rotSpeed);

    bool rotate = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);
    setRotationEnabled(rotate);

    float gScale = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::GRAVITY_SCALE, Project::Libraries::Constants::DEFAULT_GRAVITY_SCALE));
    setGravityScale(gScale);

    bool stat = luaStateWrapper.getTableBoolean(tableName, Keys::STATIC, false);
    setStatic(stat);

    bool kine = luaStateWrapper.getTableBoolean(tableName, Keys::KINEMATIC, false);
    setKinematic(kine);
  }

  void PhysicsComponent::applyForces(float deltaTime) {
    accelerationX += forceX / mass;
    accelerationY += forceY / mass;
    forceX = forceY = 0.0f;

    velocityX += accelerationX * deltaTime;
    velocityY += accelerationY * deltaTime;
    accelerationX = 0.0f;
    accelerationY = 0.0f;
  }

  void PhysicsComponent::applyResistance(float deltaTime) {
    if (!isKinematic && friction > 0.0f) {
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

    if (!isKinematic && density > 0.0f) {
      float factor = Constants::DEFAULT_WHOLE - density * deltaTime;
      if (factor < 0.0f) factor = 0.0f;
      velocityX *= factor;
      velocityY *= factor;
    }
  }

  void PhysicsComponent::clampVelocity() {
    SDL_FPoint vel{velocityX, velocityY};
    Project::Utilities::PhysicsUtils::clampVelocity(vel, Project::Libraries::Constants::TERMINAL_VELOCITY);
    velocityX = vel.x;
    velocityY = vel.y;
  }

  void PhysicsComponent::syncPositionWithComponents(float x, float y) {
    owner->setPosition(x, y);
    for (const std::string& n : owner->listComponentNames()) {
      if (auto* c = owner->getComponent(n)) {
        if (auto* pos = dynamic_cast<PositionableComponent*>(c)) {
          pos->setEntityPosition(static_cast<int>(x), static_cast<int>(y));
        }
      }
    }
  }

  void PhysicsComponent::updateRotationState(float deltaTime, bool collisionOccurred) {
    if (collisionOccurred && rotationSpeed != 0.0f) {
      angularVelocity = rotationSpeed;
    }

    angularVelocity += angularAcceleration * deltaTime;
    angularAcceleration = 0.0f;

    if (friction > 0.0f) {
      float decelR = friction * deltaTime;
      if (angularVelocity > 0.0f) {
        angularVelocity -= decelR;
        if (angularVelocity < 0.0f) angularVelocity = 0.0f;
      } else if (angularVelocity < 0.0f) {
        angularVelocity += decelR;
        if (angularVelocity > 0.0f) angularVelocity = 0.0f;
      }
    }

    rotation += angularVelocity * deltaTime;
    for (const std::string& n : owner->listComponentNames()) {
      if (auto* c = owner->getComponent(n)) {
        if (auto* rot = dynamic_cast<Project::Interfaces::Rotatable*>(c)) {
          if (auto* box = dynamic_cast<BoundingBoxComponent*>(rot)) {
            box->setRotationEnabled(rotationEnabled);
          } else if (auto* gfx = dynamic_cast<GraphicsComponent*>(rot)) {
            gfx->setRotationEnabled(rotationEnabled);
          }
          rot->setEntityRotation(rotation);
        }
      }
    }

    if (velocityX == 0.0f && velocityY == 0.0f) {
      angularVelocity = 0.0f;
    }
  }
}
