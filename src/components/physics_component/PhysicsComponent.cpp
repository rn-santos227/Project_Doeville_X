#include "PhysicsComponent.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "components/PositionableComponent.h"
#include "components/bounding_box_component/SurfaceType.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "interfaces/rotation_interface/Rotatable.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "systems/physics_system/PhysicsSystem.h"
#include "utilities/physics/PhysicsUtils.h"
#include "utilities/math/MathUtils.h"

namespace Project::Components {
  using Project::Utilities::MathUtils;
  using Project::Utilities::PhysicsUtils;
  using Project::Components::SurfaceType;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  PhysicsComponent::PhysicsComponent(Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager) {}

  void PhysicsComponent::resolveCollisionWith(PhysicsComponent* other, float restitution) {
    if (!other || !owner) return;

    auto* otherOwner = other->getOwner();
    if (!otherOwner) return;

    float nx = otherOwner->getX() - owner->getX();
    float ny = otherOwner->getY() - owner->getY();
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

    Project::Utilities::Velocity vel1{velocityX, velocityY};
    Project::Utilities::Velocity vel2{other->velocityX, other->velocityY};
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

    lastCollidedWithStatic = false;

    if (!isKinematic && gravityEnabled) {
      const float weight = mass * Constants::GRAVITY * gravityScale;
      forceX += Constants::DEFAULT_GRAVITY_DIRECTION.x * weight;
      forceY += Constants::DEFAULT_GRAVITY_DIRECTION.y * weight;
    }

    PhysicsUtils::applyForces(
      data.velocity, data.acceleration,
      data.force, mass, deltaTime
    );

    PhysicsUtils::applyResistance(
      data.velocity,
      friction, density,
      isKinematic, deltaTime
    );

    Project::Utilities::Velocity temp{velocityX, velocityY};
    PhysicsUtils::clampVelocity(temp, Constants::TERMINAL_VELOCITY);
    velocityX = temp.x;
    velocityY = temp.y;

    if (!owner) return;
    const float oldX = owner->getX();
    const float oldY = owner->getY();
    const float newX = oldX + velocityX * deltaTime;
    const float newY = oldY + velocityY * deltaTime;

    bool collisionOccurred = performContinuousCollisionDetection(newX, newY, oldX, oldY, deltaTime);
    if (rotationEnabled) {
      updateRotationState(deltaTime, collisionOccurred);
    }

    if (lastCollidedWithStatic && damping > 0.0f) {
      const float factor = std::max(0.0f, Constants::DEFAULT_WHOLE - damping * deltaTime);
      velocityX *= factor;
      velocityY *= factor;
      if (std::abs(velocityX) < Constants::DEFAULT_COLLISION_THRESHOLD) velocityX = 0.0f;
      if (std::abs(velocityY) < Constants::DEFAULT_COLLISION_THRESHOLD) velocityY = 0.0f;
    }

    forceX = forceY = 0.0f;
    accelerationX = accelerationY = 0.0f;
  }

  void PhysicsComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    float force = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FORCE, Constants::DEFAULT_PUSH_FORCE));
    setPushForce(force);

    float fric = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FRICTION, Constants::DEFAULT_FRICTION));
    setFriction(fric);

    float damp = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::DAMPING, Constants::DEFAULT_DAMPING));
    setDamping(damp);

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

  SDL_Rect PhysicsComponent::unionRect(const SDL_Rect& a, const SDL_Rect& b) const {
    const int left = std::min(a.x, b.x);
    const int top = std::min(a.y, b.y);
    const int right = std::max(a.x + a.w, b.x + b.w);
    const int bottom = std::max(a.y + a.h, b.y + b.h);
    return {left, top, right - left, bottom - top};
  }

  bool PhysicsComponent::checkBoxBoxCollisions(
    const std::vector<SDL_Rect>& myRects, const std::vector<SDL_Rect>& otherRects,
    const std::vector<Project::Utilities::OrientedBox>& myOBB, 
    const std::vector<Project::Utilities::OrientedBox>& otherOBB,
    bool myRotationEnabled, bool otherRotationEnabled,
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    float newX, float newY, float velocityDeltaX, float velocityDeltaY) {
    
    for (size_t i = 0; i < myRects.size(); ++i) {
      for (size_t j = 0; j < otherRects.size(); ++j) {
        bool collides = false;
        if (myRotationEnabled || otherRotationEnabled) {
          collides = PhysicsUtils::checkCollision(myOBB[i], otherOBB[j]);
        } else {
          collides = PhysicsUtils::checkCollision(myRects[i], otherRects[j]);
        }

        if (collides) {
          return handleCollision(
            myBox, otherBox, otherPhysics, entity,
            myRects[i], otherRects[j], newX, newY, 
            velocityDeltaX, velocityDeltaY
          );
        }
      }
    }
    return false;
  }

  bool PhysicsComponent::checkBoxCircleCollisions(
    const std::vector<SDL_Rect>& myRects, const std::vector<Project::Utilities::Circle>& otherCircles,
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    float newX, float newY, float velocityDeltaX, float velocityDeltaY) {
    
    for (const auto& rect : myRects) {
      for (const auto& circle : otherCircles) {
        if (PhysicsUtils::checkCollision(rect, circle)) {
          const SDL_FPoint offset = PhysicsUtils::getRectCircleSnapOffset(rect, circle, velocityDeltaX, velocityDeltaY);
          return handleCollision(myBox, otherBox, otherPhysics, entity, offset, newX, newY);
        }
      }
    }
    return false;
  }

  bool PhysicsComponent::checkCircleCircleCollisions(
    const std::vector<Project::Utilities::Circle>& myCircles, 
    const std::vector<Project::Utilities::Circle>& otherCircles,
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    float newX, float newY, float velocityDeltaX, float velocityDeltaY) {
    
    for (const auto& c1 : myCircles) {
      for (const auto& c2 : otherCircles) {
        if (PhysicsUtils::checkCollision(c1, c2)) {
          const SDL_FPoint offset = PhysicsUtils::getCircleSnapOffset(c1, c2, velocityDeltaX, velocityDeltaY);
          return handleCollision(myBox, otherBox, otherPhysics, entity, offset, newX, newY);
        }
      }
    }
    return false;
  }

  bool PhysicsComponent::checkCircleBoxCollisions(
    const std::vector<Project::Utilities::Circle>& myCircles, const std::vector<SDL_Rect>& otherRects,
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    float newX, float newY, float velocityDeltaX, float velocityDeltaY) {
    
    for (const auto& circle : myCircles) {
      for (const auto& rect : otherRects) {
        if (PhysicsUtils::checkCollision(rect, circle)) {
          const SDL_FPoint offset = PhysicsUtils::getCircleRectSnapOffset(circle, rect, velocityDeltaX, velocityDeltaY);
          return handleCollision(myBox, otherBox, otherPhysics, entity, offset, newX, newY);
        }
      }
    }
    return false;
  }

  bool PhysicsComponent::handleCollision(
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    const SDL_Rect& myRect, const SDL_Rect& otherRect,
    float newX, float newY, float velocityDeltaX, float velocityDeltaY) {
    
    const SDL_FPoint offset = PhysicsUtils::getSnapOffset(myRect, otherRect, velocityDeltaX, velocityDeltaY);
    return handleCollision(myBox, otherBox, otherPhysics, entity, offset, newX, newY);
  }

  bool PhysicsComponent::handleCollision(
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    const SDL_FPoint& offset, float newX, float newY) {
    const float bounce = (myBox->getRestitution() + otherBox->getRestitution()) * 0.5f;
    const float fric = (myBox->getFriction() + otherBox->getFriction()) * 0.5f;

    const bool triggerOnly = !myBox->isSolid() || !otherBox->isSolid();
    if (triggerOnly) {
      if (otherPhysics) {
        if (!myBox->isSolid()) {
          myBox->handleSurfaceInteraction(
            otherBox->getSurfaceType(),
            entity, offset, bounce, fric, velocityX, velocityY);
        }
        if (!otherBox->isSolid()) {
          auto* otherEntity = otherPhysics->getOwner();
          if (otherEntity) {
            SDL_FPoint invOffset{-offset.x, -offset.y};
            otherBox->handleSurfaceInteraction(
              myBox->getSurfaceType(), otherEntity,
              invOffset, bounce, fric,
              otherPhysics->velocityX, otherPhysics->velocityY);
          }
        }
      } else {
        if (!myBox->isSolid()) {
          myBox->handleSurfaceInteraction(
            otherBox->getSurfaceType(), entity,
            offset, bounce, fric, velocityX, velocityY);
        }
      }

      if (myBox->getSurfaceType() == SurfaceType::DESTROY_ON_HIT) {
        myBox->handleSurfaceInteraction(
          SurfaceType::DESTROY_ON_HIT,
          owner,
          offset,
          bounce,
          fric,
          velocityX,
          velocityY
        );
      }
      
      return true;
    }

    if (otherPhysics && pushForce > 0.0f && !otherPhysics->getStatic()) {
      const float pushX = velocityX * pushForce;
      const float pushY = velocityY * pushForce;
      otherPhysics->addVelocity(pushX, pushY);
      velocityX -= pushX;
      velocityY -= pushY;
    }

    const float snapX = newX + offset.x;
    const float snapY = newY + offset.y;
    syncPositionWithComponents(snapX, snapY);
    bool collidedWithStatic = false;

    if (otherPhysics) {
      if (otherPhysics->getStatic()) {
        collidedWithStatic = true;
        const auto surface = otherBox->getSurfaceType();
        if (!myBox->handleSurfaceInteraction(surface, entity, offset, bounce, fric, velocityX, velocityY)) {
          syncPositionWithComponents(newX, newY);
          return false;
        }
      } else {
         collidedWithStatic = true;
        resolveCollisionWith(otherPhysics, bounce);
        applyFriction(fric);
        otherPhysics->applyFriction(fric);
      }
    } else {
      const auto surface = otherBox->getSurfaceType();
      if (!myBox->handleSurfaceInteraction(surface, entity, offset, bounce, fric, velocityX, velocityY)) {
        syncPositionWithComponents(newX, newY);
        return false;
      }
    }

    if (collidedWithStatic && damping > 0.0f) {
      const float factor = Constants::DEFAULT_WHOLE - damping;
      velocityX *= factor;
      velocityY *= factor;
      lastCollidedWithStatic = true;
    }

    if (myBox->getSurfaceType() == SurfaceType::DESTROY_ON_HIT) {
      myBox->handleSurfaceInteraction(
        SurfaceType::DESTROY_ON_HIT,
        owner,
        offset,
        bounce,
        fric,
        velocityX,
        velocityY
      );
    }

    return true;
  }

  bool PhysicsComponent::performCollisionDetection(float newX, float newY, float oldX, float oldY, float deltaTime) {
    auto* manager = owner->getEntitiesManager();
    auto* myBox = owner->getBoundingBoxComponent();
    
    if (!manager || !myBox || !myBox->isInteractive()) {
      return false;
    }

    bool collisionOccurred = false;
    const float velocityDeltaX = velocityX * deltaTime;
    const float velocityDeltaY = velocityY * deltaTime;

    const auto& myRects = myBox->getBoxes();
    const auto& myCircles = myBox->getCircles();
    const auto& myOBB = myBox->getOrientedBoxes();
    const bool myRotationEnabled = myBox->isRotationEnabled();

    SDL_Rect myBounds{0,0,0,0};
    if (!computeBounds(myBox, myBounds)) {
      myBounds = SDL_Rect{static_cast<int>(newX), static_cast<int>(newY), 0, 0};
    }

    auto& quadtree = manager->getPhysicsSystem().getQuadTree();
    for (const auto& coll : quadtree.query(myBounds)) {
      auto* candidate = coll.physics;
      if (candidate == this) continue;
      auto* entity = coll.entity;
      auto* otherBox = coll.box;
      
      if (entity) {
        if (myBox->isIgnoring(entity->getEntityName())) continue;
        if (otherBox && otherBox->isIgnoring(owner->getEntityName())) continue;
      }

      if (!otherBox || !otherBox->isInteractive()) continue;
      if (myBox->getSurfaceType() == SurfaceType::DESTROY_ON_HIT &&
          otherBox->getSurfaceType() == SurfaceType::DESTROY_ON_HIT) {
        continue;
      }

      const auto& otherRects = otherBox->getBoxes();
      const auto& otherCircles = otherBox->getCircles();
      const auto& otherOBB = otherBox->getOrientedBoxes();
      const bool otherRotationEnabled = otherBox->isRotationEnabled();
      
      if (!broadPhaseCollisionCheck(myBox, otherBox)) {
        continue;
      }

      auto* otherPhysics = candidate;
      if (checkBoxBoxCollisions(
        myRects, otherRects, myOBB, otherOBB, 
        myRotationEnabled, otherRotationEnabled,
        myBox, otherBox, otherPhysics, entity,
        newX, newY, velocityDeltaX, velocityDeltaY)
      ) {
        collisionOccurred = true;
        if (shouldExitEarly()) break;
      }

      if (checkBoxCircleCollisions(
        myRects, otherCircles,
        myBox, otherBox, otherPhysics, entity,
        newX, newY, velocityDeltaX, velocityDeltaY)
      ) {
        collisionOccurred = true;
        if (shouldExitEarly()) break;
      }

      if (checkCircleCircleCollisions(
        myCircles, otherCircles, 
        myBox, otherBox, otherPhysics, entity,
        newX, newY, velocityDeltaX, velocityDeltaY)
      ) {
        collisionOccurred = true;
        if (shouldExitEarly()) break;
      }

      if (checkCircleBoxCollisions(
        myCircles, otherRects, 
        myBox, otherBox, otherPhysics, entity,
        newX, newY, velocityDeltaX, velocityDeltaY)
      ) {
        collisionOccurred = true;
        if (shouldExitEarly()) break;
      }
    }

    return collisionOccurred;
  }

  bool PhysicsComponent::performContinuousCollisionDetection(float newX, float newY, float oldX, float oldY, float deltaTime) {
    int steps = static_cast<int>(std::ceil(std::max(std::abs(newX - oldX), std::abs(newY - oldY)) / Constants::DEFAULT_CELL_SIZE));
    if (steps < Constants::INT_ONE) steps = Constants::INT_ONE;
    float stepX = (newX - oldX) / static_cast<float>(steps);
    float stepY = (newY - oldY) / static_cast<float>(steps);
    float cx = oldX;
    float cy = oldY;
    bool collided = false;
    for (int i = 0; i < steps; ++i) {
      cx += stepX;
      cy += stepY;
      syncPositionWithComponents(cx, cy);
      collided = performCollisionDetection(cx, cy, oldX, oldY, deltaTime / steps);
      if (collided) {
        break;
      }
      oldX = cx;
      oldY = cy;
    }
    return collided;
  }

  bool PhysicsComponent::broadPhaseCollisionCheck(BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox) {
    const auto& myRects = myBox->getBoxes();
    const auto& otherRects = otherBox->getBoxes();
    
    if (myRects.empty() || otherRects.empty()) return true;
    
    SDL_Rect myBounds = myRects[0];
    for (size_t i = 1; i < myRects.size(); ++i) {
      myBounds = unionRect(myBounds, myRects[i]);
    }
    
    SDL_Rect otherBounds = otherRects[0];
    for (size_t i = 1; i < otherRects.size(); ++i) {
      otherBounds = unionRect(otherBounds, otherRects[i]);
    }
    
    const int padding = Constants::DEFAULT_COLLISION_PADDING;
    myBounds.x -= padding;
    myBounds.y -= padding;
    myBounds.w += Constants::INDEX_TWO * padding;
    myBounds.h += Constants::INDEX_TWO * padding;
    
    return SDL_HasIntersection(&myBounds, &otherBounds);
  }

  bool PhysicsComponent::computeBounds(BoundingBoxComponent* box, SDL_Rect& bounds) const {
    if (!box) return false;
    bool hasBounds = false;

    const auto& rects = box->getBoxes();
    for (size_t i = 0; i < rects.size(); ++i) {
      if (!hasBounds) {
        bounds = rects[i];
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, rects[i]);
      }
    }

    const auto& circles = box->getCircles();
    for (const auto& c : circles) {
      SDL_Rect r{
        c.x - c.r, c.y - c.r,
        c.r * Constants::CIRCLE_DIAMETER_MULTIPLIER,
        c.r * Constants::CIRCLE_DIAMETER_MULTIPLIER
      };

      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, r);
      }
    }

    const auto& polys = box->getPolygons();
    for (const auto& p : polys) {
      SDL_Rect r = Project::Utilities::GeometryUtils::polygonBounds(p);
      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, r);
      }
    }

    const auto& caps = box->getCapsules();
    for (const auto& c : caps) {
      SDL_Rect r = Project::Utilities::GeometryUtils::capsuleBounds(c);
      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, r);
      }
    }

    return hasBounds;
  }

  bool PhysicsComponent::shouldExitEarly() {
    return std::abs(velocityX) < 0.1f && std::abs(velocityY) < 0.1f;
  }

  void PhysicsComponent::applyFriction(float fric) {
    const float frictionFactor = Constants::DEFAULT_WHOLE - fric;
    velocityX *= frictionFactor;
    velocityY *= frictionFactor;
  }

  void PhysicsComponent::syncPositionWithComponents(float x, float y) {
    owner->setPosition(x, y);
    const auto& componentNames = owner->listComponentNames();
    for (const std::string& name : componentNames) {
      if (auto* component = owner->getComponent(name)) {
        if (auto* positionable = dynamic_cast<PositionableComponent*>(component)) {
          positionable->setEntityPosition(static_cast<int>(x), static_cast<int>(y));
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
      const float deceleration = friction * deltaTime;
      if (angularVelocity > 0.0f) {
        angularVelocity = std::max(0.0f, angularVelocity - deceleration);
      } else if (angularVelocity < 0.0f) {
        angularVelocity = std::min(0.0f, angularVelocity + deceleration);
      }
    }

    rotation += angularVelocity * deltaTime;

    const auto& componentNames = owner->listComponentNames();
    for (const std::string& name : componentNames) {
      if (auto* component = owner->getComponent(name)) {
        if (auto* rotatable = dynamic_cast<Project::Interfaces::Rotatable*>(component)) {
          if (auto* box = dynamic_cast<BoundingBoxComponent*>(rotatable)) {
            box->setRotationEnabled(rotationEnabled);
          } else if (auto* gfx = dynamic_cast<GraphicsComponent*>(rotatable)) {
            gfx->setRotationEnabled(rotationEnabled);
          }
          rotatable->setEntityRotation(rotation);
        }
      }
    }

    if (velocityX == 0.0f && velocityY == 0.0f) {
      angularVelocity = 0.0f;
    }
  }
}
