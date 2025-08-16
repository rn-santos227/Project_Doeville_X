#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "PhysicsData.h"

#include "components/BaseComponent.h"
#include "components/bounding_box_component/SurfaceType.h"
#include "libraries/constants/Constants.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class BoundingBoxComponent;
}

namespace Project::Components {

  class PhysicsComponent : public BaseComponent {
  public:

    explicit PhysicsComponent(Project::Utilities::LogsManager& logsManager);
    ~PhysicsComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::PHYSICS; }

    void resolveCollisionWith(PhysicsComponent* other, float restitution);

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

    void setTickRate(float rate) { data.tickRate = rate; }
    float getTickRate() const { return data.tickRate; }

    void setVelocity(float vx, float vy) { data.setVelocity(vx, vy); }
    void addVelocity(float vx, float vy) { data.addVelocity(vx, vy); }
    
    float getVelocityX() const { return data.velocity.x; }
    float getVelocityY() const { return data.velocity.y; }

    void setAcceleration(float ax, float ay) { data.setAcceleration(ax, ay); }
    void addAcceleration(float ax, float ay) { data.addAcceleration(ax, ay); }

    float getAccelerationX() const { return data.acceleration.x; }
    float getAccelerationY() const { return data.acceleration.y; }

    void setDamping(float d) { data.damping = d; }
    float getDamping() const { return data.damping; }

    void setFriction(float f) { data.friction = f; }
    float getFriction() const { return data.friction; }

    void setPushForce(float f) { data.pushForce = f; }
    float getPushForce() const { return data.pushForce; }

    void setDensity(float d) { data.density = d; }
    float getDensity() const { return data.density; }
    
    void setRotationEnabled(bool enabled) { data.rotationEnabled = enabled; }
    bool isRotationEnabled() const { return data.rotationEnabled; }

    void setRotation(float r) { data.rotation = r; }
    float getRotation() const { return data.rotation; }

    void setAngularVelocity(float av) { data.angularVelocity = av; }
    float getAngularVelocity() const { return data.angularVelocity; }

    void setAngularAcceleration(float aa) { data.angularAcceleration = aa; }
    float getAngularAcceleration() const { return data.angularAcceleration; }

    void setGravityEnabled(bool enabled) { data.gravityEnabled = enabled; }
    bool isGravityEnabled() const { return data.gravityEnabled; }

    void setGravityScale(float scale) { data.gravityScale = scale; }
    float getGravityScale() const { return data.gravityScale; }

    void setStatic(bool s) { data.isStatic = s; }
    bool getStatic() const { return data.isStatic; }

    void setKinematic(bool k) { data.isKinematic = k; }
    bool isKinematicBody() const { return data.isKinematic; }

    void setRotationSpeed(float rs) { data.rotationSpeed = rs; }
    float getRotationSpeed() const { return data.rotationSpeed; }

    void setRestitution(float r) { data.restitution = r; }
    float getRestitution() const { return data.restitution; }

    void setMass(float m) { data.mass = m; }
    float getMass() const { return data.mass; }
    float getWeight() const { return data.getWeight(); }

    void addAngularVelocity(float av) { data.addAngularVelocity(av); }
    void addAngularAcceleration(float aa) { data.addAngularAcceleration(aa); }

    void applyForce(float fx, float fy) {data.applyForce(fx, fy);}
    bool performContinuousCollisionDetection(float newX, float newY, float oldX, float oldY, float deltaTime);

    void setUpdateFrequency(UpdateFrequency freq) { data.updateFrequency = freq; }
    UpdateFrequency getUpdateFrequency() const { return data.updateFrequency; }

  private:
    Project::Entities::Entity* owner = nullptr;
    PhysicsData data;

    float& forceX = data.force.x;
    float& forceY = data.force.y;

    float& accelerationX = data.acceleration.x;
    float& accelerationY = data.acceleration.y;
    float& angularAcceleration = data.angularAcceleration;
    float& angularVelocity = data.angularVelocity;
    float& rotation = data.rotation;
    float& velocityX = data.velocity.x;
    float& velocityY = data.velocity.y;

    float& friction = data.friction;
    float& damping = data.damping;
    float& density = data.density;
    float& mass = data.mass;
    float& pushForce = data.pushForce;
    float& restitution = data.restitution;
    float& rotationSpeed = data.rotationSpeed;
    float& gravityScale = data.gravityScale;

    bool& rotationEnabled = data.rotationEnabled;
    bool& gravityEnabled = data.gravityEnabled;
    bool& isStatic = data.isStatic;
    bool& isKinematic = data.isKinematic;
    bool lastCollidedWithStatic = false;

    SDL_FRect unionRect(const SDL_FRect& a, const SDL_FRect& b) const;

    bool checkBoxBoxCollisions(
      const std::vector<SDL_FRect>& myRects, const std::vector<SDL_FRect>& otherRects,
      const std::vector<Project::Utilities::OrientedBox>& myOBB, 
      const std::vector<Project::Utilities::OrientedBox>& otherOBB,
      bool myRotationEnabled, bool otherRotationEnabled,
      Project::Components::BoundingBoxComponent* myBox,
      Project::Components::BoundingBoxComponent* otherBox,
      PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
      float newX, float newY, float velocityDeltaX, float velocityDeltaY
    );

    bool checkBoxCircleCollisions(
      const std::vector<SDL_FRect>& myRects, const std::vector<Project::Utilities::Circle>& otherCircles,
      Project::Components::BoundingBoxComponent* myBox,
      Project::Components::BoundingBoxComponent* otherBox,
      PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
      float newX, float newY, float velocityDeltaX, float velocityDeltaY
    );
    
    bool checkCircleCircleCollisions(
      const std::vector<Project::Utilities::Circle>& myCircles, 
      const std::vector<Project::Utilities::Circle>& otherCircles,
      Project::Components::BoundingBoxComponent* myBox,
      Project::Components::BoundingBoxComponent* otherBox,
      PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
      float newX, float newY, float velocityDeltaX, float velocityDeltaY
    );
    
    bool checkCircleBoxCollisions(    
      const std::vector<Project::Utilities::Circle>& myCircles, const std::vector<SDL_FRect>& otherRects,
      Project::Components::BoundingBoxComponent* myBox,
      Project::Components::BoundingBoxComponent* otherBox,
      PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
      float newX, float newY, float velocityDeltaX, float velocityDeltaY
    );

    bool handleCollision(
      Project::Components::BoundingBoxComponent* myBox,
      Project::Components::BoundingBoxComponent* otherBox,
      PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
      const SDL_FRect& myRect, const SDL_FRect& otherRect,
      float newX, float newY, float velocityDeltaX, float velocityDeltaY
    );

    bool handleCollision(
      Project::Components::BoundingBoxComponent* myBox,
      Project::Components::BoundingBoxComponent* otherBox,
      PhysicsComponent* otherPhysics, Project::Entities::Entity* entity,
      const SDL_FPoint& offset, float newX, float newY
    );


    bool performCollisionDetection(float newX, float newY, float oldX, float oldY, float deltaTime);
    bool broadPhaseCollisionCheck(
      Project::Components::BoundingBoxComponent* myBox, 
      Project::Components::BoundingBoxComponent* otherBox
    );

    bool computeBounds(Project::Components::BoundingBoxComponent* box, SDL_FRect& bounds) const;
    bool shouldExitEarly();
    
    void applyFriction(float fric);
    void syncPositionWithComponents(float x, float y);
    void updateRotationState(float deltaTime, bool collisionOccurred);
  };
}

#endif
