#include "PhysicsComponent.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
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
    auto stepUpdate = [&](float step) {
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
        data.force, mass, step
      );

      PhysicsUtils::applyResistance(
        data.velocity,
        friction, density,
        isKinematic, step
      );

      Project::Utilities::Velocity temp{velocityX, velocityY};
      PhysicsUtils::clampVelocity(temp, Constants::TERMINAL_VELOCITY);
      velocityX = temp.x;
      velocityY = temp.y;

      if (!owner) return;
      const float oldX = owner->getX();
      const float oldY = owner->getY();
      const float newX = oldX + velocityX * step;
      const float newY = oldY + velocityY * step;

      bool collisionOccurred = performContinuousCollisionDetection(newX, newY, oldX, oldY, step);
      if (rotationEnabled) {
        updateRotationState(step, collisionOccurred);
      }

      if (lastCollidedWithStatic && damping > 0.0f) {
        const float factor = std::max(0.0f, Constants::DEFAULT_WHOLE - damping * step);
        velocityX *= factor;
        velocityY *= factor;
        if (std::abs(velocityX) < Constants::DEFAULT_COLLISION_THRESHOLD) velocityX = 0.0f;
        if (std::abs(velocityY) < Constants::DEFAULT_COLLISION_THRESHOLD) velocityY = 0.0f;
      }

      forceX = forceY = 0.0f;
      accelerationX = accelerationY = 0.0f;
    };

    if (data.tickRate > 0.0f) {
      data.tickAccumulator += deltaTime;
      while (data.tickAccumulator >= data.tickRate) {
        float step = data.tickRate;
        data.tickAccumulator -= data.tickRate;
        stepUpdate(step);
      }
      if (data.tickAccumulator > 0.0f) {
        float step = data.tickAccumulator;
        stepUpdate(step);
        data.tickAccumulator = 0.0f;
      }
      return;
    }

    stepUpdate(deltaTime);
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

  SDL_FRect PhysicsComponent::unionRect(const SDL_FRect& a, const SDL_FRect& b) const {
    const float left = std::min(a.x, b.x);
    const float top = std::min(a.y, b.y);
    const float right = std::max(a.x + a.w, b.x + b.w);
    const float bottom = std::max(a.y + a.h, b.y + b.h);
    return {left, top, right - left, bottom - top};
  }

  bool PhysicsComponent::checkBoxBoxCollisions(
    const std::vector<SDL_FRect>& myRects, const std::vector<SDL_FRect>& otherRects,
    const std::vector<Project::Utilities::OrientedBox>& myOBB,
    const std::vector<Project::Utilities::OrientedBox>& otherOBB,
    bool myRotationEnabled, bool otherRotationEnabled,
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    float newX, float newY, float velocityDeltaX, float velocityDeltaY) {

    auto rectToOBB = [](const SDL_FRect& r) {
      Project::Utilities::OrientedBox obb;
      obb.corners[Constants::INDEX_ZERO] = {r.x, r.y};
      obb.corners[Constants::INDEX_ONE] = {r.x + r.w, r.y};
      obb.corners[Constants::INDEX_TWO] = {r.x + r.w, r.y + r.h};
      obb.corners[Constants::INDEX_THREE] = {r.x, r.y + r.h};
      return obb;
    };
    
    for (size_t i = 0; i < myRects.size(); ++i) {
      for (size_t j = 0; j < otherRects.size(); ++j) {
        bool collides = false;
        if (myRotationEnabled && otherRotationEnabled) {
          collides = PhysicsUtils::checkCollision(myOBB[i], otherOBB[j]);
        } else if (myRotationEnabled) {
          auto orect = rectToOBB(otherRects[j]);
          collides = PhysicsUtils::checkCollision(myOBB[i], orect);
        } else if (otherRotationEnabled) {
          auto orect = rectToOBB(myRects[i]);
          collides = PhysicsUtils::checkCollision(orect, otherOBB[j]);
        } else {
          collides = PhysicsUtils::checkCollision(myRects[i], otherRects[j]);
        }

        if (collides) {
          SDL_FPoint offset;
          if (myRotationEnabled || otherRotationEnabled) {
            auto oA = myRotationEnabled ? myOBB[i] : rectToOBB(myRects[i]);
            auto oB = otherRotationEnabled ? otherOBB[j] : rectToOBB(otherRects[j]);
            offset = PhysicsUtils::getOBBSnapOffset(oA, oB, velocityDeltaX, velocityDeltaY);
          } else {
            offset = PhysicsUtils::getSnapOffset(myRects[i], otherRects[j], velocityDeltaX, velocityDeltaY);
          }
          
          return handleCollision(
            myBox, otherBox, otherPhysics, entity, offset, newX, newY
          );
        }
      }
    }
    return false;
  }

  bool PhysicsComponent::checkBoxCircleCollisions(
    const std::vector<SDL_FRect>& myRects, const std::vector<Project::Utilities::Circle>& otherCircles,
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
    const std::vector<Project::Utilities::Circle>& myCircles, const std::vector<SDL_FRect>& otherRects,
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
    const SDL_FRect& myRect, const SDL_FRect& otherRect,
    float newX, float newY, float velocityDeltaX, float velocityDeltaY) {
    
    const SDL_FPoint offset = PhysicsUtils::getSnapOffset(myRect, otherRect, velocityDeltaX, velocityDeltaY);
    return handleCollision(myBox, otherBox, otherPhysics, entity, offset, newX, newY);
  }

  bool PhysicsComponent::handleCollision(
    BoundingBoxComponent* myBox, BoundingBoxComponent* otherBox,
    PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
    const SDL_FPoint& offset, float newX, float newY) {
    const float bounce = (myBox->getRestitution() + otherBox->getRestitution()) * Constants::DEFAULT_HALF;
    const float fric = (myBox->getFriction() + otherBox->getFriction()) * Constants::DEFAULT_HALF;

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

    const bool dynamicCollision = otherPhysics && !otherPhysics->getStatic();
    const bool primaryHandler = !dynamicCollision || this < otherPhysics;

    if (dynamicCollision && !primaryHandler) {
      return true;
    }

    if (otherPhysics && pushForce > 0.0f && primaryHandler && !otherPhysics->getStatic()) {
      const float pushX = velocityX * pushForce;
      const float pushY = velocityY * pushForce;
      otherPhysics->addVelocity(pushX, pushY);
      velocityX -= pushX;
      velocityY -= pushY;
    }

    float snapX = newX + offset.x;
    float snapY = newY + offset.y;

    if (otherPhysics && !otherPhysics->getStatic()) {
      const float halfOffsetX = offset.x * Constants::DEFAULT_HALF;
      const float halfOffsetY = offset.y * Constants::DEFAULT_HALF;

      snapX = newX + halfOffsetX;
      snapY = newY + halfOffsetY;
      syncPositionWithComponents(snapX, snapY);

      if (auto* otherOwner = otherPhysics->getOwner()) {
        const float otherSnapX = otherOwner->getX() - halfOffsetX;
        const float otherSnapY = otherOwner->getY() - halfOffsetY;
        otherPhysics->syncPositionWithComponents(otherSnapX, otherSnapY);
      }
    } else {
      syncPositionWithComponents(snapX, snapY);
    }
    
    bool collidedWithStatic = false;

    if (otherPhysics) {
      if (otherPhysics->getStatic()) {
        collidedWithStatic = true;
        const auto surface = otherBox->getSurfaceType();
        if (!myBox->handleSurfaceInteraction(surface, entity, offset, bounce, fric, velocityX, velocityY)) {
          syncPositionWithComponents(newX, newY);
          return false;
        }
      } else if (primaryHandler) {
        resolveCollisionWith(otherPhysics, bounce);
        applyFriction(fric);
        otherPhysics->applyFriction(fric);

        if (std::abs(velocityX) < Constants::DEFAULT_COLLISION_THRESHOLD) velocityX = 0.0f;
        if (std::abs(velocityY) < Constants::DEFAULT_COLLISION_THRESHOLD) velocityY = 0.0f;
        if (std::abs(otherPhysics->velocityX) < Constants::DEFAULT_COLLISION_THRESHOLD) {
          otherPhysics->velocityX = 0.0f;
        }
        if (std::abs(otherPhysics->velocityY) < Constants::DEFAULT_COLLISION_THRESHOLD) {
          otherPhysics->velocityY = 0.0f;
        }
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

    // const auto& myRects = myBox->getBoxes();
    // const auto& myCircles = myBox->getCircles();
    // const auto& myOBB = myBox->getOrientedBoxes();
    // const bool myRotationEnabled = myBox->isRotationEnabled();   
    std::vector<SDL_FRect> myProxyRects;
    std::vector<Project::Utilities::Circle> emptyCircles;
    std::vector<Project::Utilities::OrientedBox> emptyOBB;
    const auto& myRects = myBox->usesProxy() ? (myProxyRects.push_back(myBox->getProxyAABB()), myProxyRects) : myBox->getBoxes();
    const auto& myCircles = myBox->usesProxy() ? emptyCircles : myBox->getCircles();
    const auto& myOBB = myBox->usesProxy() ? emptyOBB : myBox->getOrientedBoxes();
    const bool myRotationEnabled = myBox->usesProxy() ? false : myBox->isRotationEnabled();

    SDL_FRect myBounds{0.f, 0.f, 0.f, 0.f};
    if (!computeBounds(myBox, myBounds)) {
      myBounds = SDL_FRect{newX, newY, 0.f, 0.f};
    }

    auto& physSystem = manager->getPhysicsSystem();
    auto& bvh = physSystem.getBVH();
    // auto& quadtree = physSystem.getQuadTree();
    auto queryStart = std::chrono::high_resolution_clock::now();
   
    auto candidates = bvh.query(myBounds);
    // auto candidates = quadtree.query(myBounds);
    auto queryEnd = std::chrono::high_resolution_clock::now();
    physSystem.recordSpatialQuery(std::chrono::duration<float, std::milli>(queryEnd - queryStart).count());

    std::sort(candidates.begin(), candidates.end(),
      [](const Project::Utilities::Collider& a, const Project::Utilities::Collider& b) {
        if (a.physics != b.physics) return a.physics < b.physics;
        if (a.box != b.box) return a.box < b.box;
        return a.entity < b.entity;
      });
      
    candidates.erase(std::unique(candidates.begin(), candidates.end(),
      [](const Project::Utilities::Collider& a, const Project::Utilities::Collider& b) {
        return a.physics == b.physics && a.box == b.box && a.entity == b.entity;
      }), candidates.end());

    for (const auto& coll : candidates) {
      auto* candidate = coll.physics;
      if (candidate == this) continue;
      auto* entity = candidate ? candidate->getOwner() : coll.entity;
      auto* otherBox = coll.box;
      if (!entity || !otherBox) continue;
      
      if (myBox->isIgnoring(entity->getEntityName())) continue;
      if (otherBox->isIgnoring(owner->getEntityName())) continue;

      if (!otherBox->isInteractive()) continue;
      if (myBox->getSurfaceType() == SurfaceType::DESTROY_ON_HIT &&
          otherBox->getSurfaceType() == SurfaceType::DESTROY_ON_HIT) {
        continue;
      }

      // const auto& otherRects = otherBox->getBoxes();
      // const auto& otherCircles = otherBox->getCircles();
      // const auto& otherOBB = otherBox->getOrientedBoxes();
      // const bool otherRotationEnabled = otherBox->isRotationEnabled();
      std::vector<SDL_FRect> otherProxyRects;
      std::vector<Project::Utilities::Circle> otherEmptyCircles;
      std::vector<Project::Utilities::OrientedBox> otherEmptyOBB;
      const auto& otherRects = otherBox->usesProxy() ? (otherProxyRects.push_back(otherBox->getProxyAABB()), otherProxyRects) : otherBox->getBoxes();
      const auto& otherCircles = otherBox->usesProxy() ? otherEmptyCircles : otherBox->getCircles();
      const auto& otherOBB = otherBox->usesProxy() ? otherEmptyOBB : otherBox->getOrientedBoxes();
      const bool otherRotationEnabled = otherBox->usesProxy() ? false : otherBox->isRotationEnabled();
      
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
    SDL_FRect myBounds{0.f, 0.f, 0.f, 0.f};
    SDL_FRect otherBounds{0.f, 0.f, 0.f ,0.f};

    if (myBox->usesProxy()) {
      myBounds = myBox->getProxyAABB();
    } else if (!computeBounds(myBox, myBounds)) {
      return true;
    }
    
    if (otherBox->usesProxy()) {
      otherBounds = otherBox->getProxyAABB();
    } else if (!computeBounds(otherBox, otherBounds)) {
      return true;
    }
    
    const float padding = static_cast<float>(Constants::DEFAULT_COLLISION_PADDING);
    myBounds.x -= padding;
    myBounds.y -= padding;
    myBounds.w += Constants::INDEX_TWO * padding;
    myBounds.h += Constants::INDEX_TWO * padding;
    
    return SDL_HasIntersectionF(&myBounds, &otherBounds);
  }

  bool PhysicsComponent::computeBounds(BoundingBoxComponent* box, SDL_FRect& bounds) const {
    if (!box) return false;
    if (box->usesProxy()) {
      bounds = box->getProxyAABB();
      return true;
    }
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
      SDL_FRect r{
        c.x - c.r,
        c.y - c.r,
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
      SDL_FRect r = Project::Utilities::GeometryUtils::polygonBounds(p);
      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, r);
      }
    }

    const auto& caps = box->getCapsules();
    for (const auto& c : caps) {
      SDL_FRect r = Project::Utilities::GeometryUtils::capsuleBounds(c);
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
    if (!owner) return;
    owner->setPosition(x, y);
    const auto& componentNames = owner->listComponentNames();
    for (const std::string& name : componentNames) {
      if (auto* component = owner->getComponent(name)) {
        if (auto* positionable = dynamic_cast<PositionableComponent*>(component)) {
          positionable->setEntityPosition(x, y);
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
