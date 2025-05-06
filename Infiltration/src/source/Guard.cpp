#include "Guard.h"
#include "Level.h"
#include <OgreMath.h>
#include <OgreManualObject.h>
#include <OgreMaterialManager.h>

Guard::Guard(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, 
             Level* level, const Ogre::Vector3& startPosition) 
    : mSceneMgr(sceneMgr)
    , mLevel(level)
    , mState(GuardState::PATROLLING)
    , mAlertLevel(0.0f)
    , mStateTime(0.0f)
    , mVisionConeAngle(Ogre::Degree(60).valueRadians())
    , mVisionRange(15.0f)
    , mWalkSpeed(2.0f)
    , mRunSpeed(5.0f)
    , mCurrentWaypoint(0)
    , mTargetPosition(startPosition)
{
    // Create the guard's visual representation
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNode->setPosition(startPosition);
    
    // Create entity using a simple mesh
    mEntity = mSceneMgr->createEntity("guard_" + std::to_string(reinterpret_cast<uintptr_t>(this)), "guard.mesh");
    mSceneNode->attachObject(mEntity);
    
    // Create the physics body for the guard
    Ogre::Vector3 size(1.0f, 2.0f, 1.0f); // Guard size
    OgreBulletCollisions::BoxCollisionShape* boxShape = new OgreBulletCollisions::BoxCollisionShape(size);
    
    float mass = 80.0f; // Guard mass in kg
    mBody = new OgreBulletDynamics::RigidBody(
        "guardBody_" + std::to_string(reinterpret_cast<uintptr_t>(this)), 
        world, 
        10, // Collision group
        10  // Collision mask
    );
    
    mBody->setShape(mSceneNode, boxShape, 0.6f, 0.6f, mass, startPosition, Ogre::Quaternion::IDENTITY);
    mBody->disableDeactivation();
    mBody->getBulletRigidBody()->setAngularFactor(btVector3(0, 0, 0)); // Prevent rotation
    
    // Create the vision cone
    createVisionCone();
}

Guard::~Guard() {
    // Clean up physics
    if (mBody) {
        delete mBody->getCollisionShape();
        delete mBody;
    }
    
    // Clean up vision cone
    if (mVisionConeNode) {
        mVisionConeNode->detachAllObjects();
        mSceneMgr->destroySceneNode(mVisionConeNode);
    }
    
    if (mVisionConeEntity) {
        mSceneMgr->destroyEntity(mVisionConeEntity);
    }
    
    // Clean up guard entity
    if (mSceneNode) {
        mSceneNode->detachAllObjects();
        mSceneMgr->destroySceneNode(mSceneNode);
    }
    
    if (mEntity) {
        mSceneMgr->destroyEntity(mEntity);
    }
}

void Guard::update(float deltaTime) {
    // Update state based on time in current state
    updateState(deltaTime);
    
    // Update movement based on current state
    updateMovement(deltaTime);
    
    // Update vision cone position and orientation
    updateVisionCone();
    
    // Update vision cone color based on alert level
    updateVisionConeColor();
    
    // Gradually decrease alert level over time when not fully alerted
    if (mState != GuardState::ALERTED && mAlertLevel > 0.0f) {
        mAlertLevel = std::max(0.0f, mAlertLevel - deltaTime * 0.1f);
    }
}

void Guard::setPatrolPath(const std::vector<Ogre::Vector3>& waypoints) {
    mPatrolWaypoints = waypoints;
    if (!mPatrolWaypoints.empty()) {
        mCurrentWaypoint = 0;
        mTargetPosition = mPatrolWaypoints[mCurrentWaypoint];
    }
}

bool Guard::canSeePlayer(const Ogre::Vector3& playerPosition) const {
    // Get guard position and direction
    Ogre::Vector3 guardPosition = mSceneNode->getPosition();
    Ogre::Vector3 guardDirection = mSceneNode->getOrientation() * Ogre::Vector3::UNIT_Z;
    
    // Calculate vector to player
    Ogre::Vector3 toPlayer = playerPosition - guardPosition;
    float distanceToPlayer = toPlayer.normalise();
    
    // Check if player is within vision range
    if (distanceToPlayer > mVisionRange) {
        return false;
    }
    
    // Check if player is within vision cone angle
    float dotProduct = guardDirection.dotProduct(toPlayer);
    float angleToPlayer = std::acos(dotProduct);
    if (angleToPlayer > mVisionConeAngle / 2.0f) {
        return false;
    }
    
    // Perform raycast to check if there are obstacles between guard and player
    Ogre::Ray ray(guardPosition, toPlayer);
    std::pair<bool, float> rayResult = mLevel->raycastToPoint(ray, distanceToPlayer);
    
    // If the ray hits something before reaching the player's distance, then the player is occluded
    if (rayResult.first && rayResult.second < distanceToPlayer - 0.5f) {
        return false;
    }
    
    return true;
}

void Guard::reactToNoise(const Ogre::Vector3& noisePosition, float intensity) {
    // Calculate distance to noise
    float distance = (mSceneNode->getPosition() - noisePosition).length();
    
    // Calculate how much this noise should affect the guard based on distance
    float effectIntensity = intensity * (1.0f - (distance / 20.0f));
    
    if (effectIntensity <= 0) {
        return; // Too far to hear
    }
    
    // Increase alert level based on noise intensity
    mAlertLevel += effectIntensity;
    
    // If noise is significant, change state
    if (effectIntensity > 0.3f && mState == GuardState::PATROLLING) {
        mLastKnownPlayerPosition = noisePosition;
        transitionToState(GuardState::SUSPICIOUS);
    } else if (effectIntensity > 0.7f) {
        mLastKnownPlayerPosition = noisePosition;
        transitionToState(GuardState::SEARCHING);
    }
}

void Guard::updateMovement(float deltaTime) {
    // Get current position
    Ogre::Vector3 currentPosition = mSceneNode->getPosition();
    
    // Calculate next position
    Ogre::Vector3 nextPosition = calculateNextPosition(deltaTime);
    
    // Calculate direction vector
    Ogre::Vector3 direction = nextPosition - currentPosition;
    float distance = direction.normalise();
    
    // Only rotate if we're actually moving
    if (distance > 0.01f) {
        // Calculate the orientation to face the movement direction
        Ogre::Vector3 src = mSceneNode->getOrientation() * Ogre::Vector3::UNIT_Z;
        Ogre::Quaternion quat = src.getRotationTo(direction);
        mSceneNode->rotate(quat);
        
        // Apply movement to physics body
        float speed = (mState == GuardState::ALERTED) ? mRunSpeed : mWalkSpeed;
        Ogre::Vector3 velocity = direction * speed;
        mBody->getBulletRigidBody()->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    } else {
        // Stop movement if we reached the destination
        mBody->getBulletRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
    }
    
    // Update scene node position from physics
    btVector3 btPos = mBody->getBulletRigidBody()->getCenterOfMassPosition();
    currentPosition = Ogre::Vector3(btPos.x(), btPos.y(), btPos.z());
    mSceneNode->setPosition(currentPosition);
}

Ogre::Vector3 Guard::calculateNextPosition(float deltaTime) {
    switch (mState) {
        case GuardState::PATROLLING:
            // Move towards the current waypoint
            if (!mPatrolWaypoints.empty()) {
                Ogre::Vector3 toWaypoint = mPatrolWaypoints[mCurrentWaypoint] - mSceneNode->getPosition();
                
                // If close enough to current waypoint, move to next
                if (toWaypoint.length() < 0.5f) {
                    mCurrentWaypoint = (mCurrentWaypoint + 1) % mPatrolWaypoints.size();
                    return mPatrolWaypoints[mCurrentWaypoint];
                }
                
                return mPatrolWaypoints[mCurrentWaypoint];
            }
            break;
            
        case GuardState::SUSPICIOUS:
            // Slow movement or slight look around
            return mSceneNode->getPosition();
            
        case GuardState::ALERTED:
        case GuardState::SEARCHING:
            // Move towards last known player position
            return mLastKnownPlayerPosition;
    }
    
    return mSceneNode->getPosition();
}

void Guard::updateState(float deltaTime) {
    mStateTime += deltaTime;
    
    switch (mState) {
        case GuardState::SUSPICIOUS:
            // After being suspicious for a while, return to patrol
            if (mStateTime > 5.0f) {
                transitionToState(GuardState::PATROLLING);
            }
            break;
            
        case GuardState::SEARCHING:
            // After searching for a while with no success, return to patrol
            if (mStateTime > 15.0f) {
                transitionToState(GuardState::PATROLLING);
            }
            break;
            
        case GuardState::ALERTED:
            // Stay alerted for a while then go to searching
            if (mStateTime > 10.0f) {
                transitionToState(GuardState::SEARCHING);
            }
            break;
            
        default:
            break;
    }
    
    // Update state based on alert level
    if (mAlertLevel >= 1.0f && mState != GuardState::ALERTED) {
        transitionToState(GuardState::ALERTED);
    } else if (mAlertLevel >= 0.5f && mState == GuardState::PATROLLING) {
        transitionToState(GuardState::SUSPICIOUS);
    }
}

void Guard::transitionToState(GuardState newState) {
    mState = newState;
    mStateTime = 0.0f;
    
    // Reset alert level for certain transitions
    if (newState == GuardState::PATROLLING) {
        mAlertLevel = 0.0f;
    }
}

void Guard::createVisionCone() {
    // Create a scene node for the vision cone
    mVisionConeNode = mSceneNode->createChildSceneNode(Ogre::Vector3(0, 0, 0));
    
    // Create a manual object for the vision cone
    Ogre::ManualObject* manual = mSceneMgr->createManualObject("VisionCone_" + std::to_string(reinterpret_cast<uintptr_t>(this)));
    
    // Create material for the vision cone
    Ogre::String matName = "VisionConeMaterial_" + std::to_string(reinterpret_cast<uintptr_t>(this));
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(matName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    material->getTechnique(0)->getPass(0)->setDiffuse(0.0f, 1.0f, 0.0f, 0.3f); // Green, semi-transparent
    material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    
    // Create a simple cone shape
    manual->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_FAN);
    
    // Center of the fan
    manual->position(0, 0, 0);
    manual->colour(0.0f, 1.0f, 0.0f, 0.0f); // Transparent at center
    
    // Create the cone perimeter points
    int segments = 20;
    float halfAngle = mVisionConeAngle / 2.0f;
    
    for (int i = 0; i <= segments; ++i) {
        float angle = -halfAngle + (2 * halfAngle * i) / segments;
        float x = mVisionRange * Ogre::Math::Sin(angle);
        float z = mVisionRange * Ogre::Math::Cos(angle);
        manual->position(x, 0, z);
        manual->colour(0.0f, 1.0f, 0.0f, 0.3f); // Semi-transparent at edges
    }
    
    manual->end();
    
    // Attach the manual object to the vision cone node
    mVisionConeNode->attachObject(manual);
}

void Guard::updateVisionCone() {
    // Update the vision cone position and orientation to match the guard
    mVisionConeNode->setPosition(0, 0.5f, 0); // Slightly above ground level
}

void Guard::updateVisionConeColor() {
    // Change the color based on the guard's state
    Ogre::String matName = "VisionConeMaterial_" + std::to_string(reinterpret_cast<uintptr_t>(this));
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(matName);
    
    if (material) {
        switch (mState) {
            case GuardState::PATROLLING:
                material->getTechnique(0)->getPass(0)->setDiffuse(0.0f, 1.0f, 0.0f, 0.3f); // Green
                break;
                
            case GuardState::SUSPICIOUS:
                material->getTechnique(0)->getPass(0)->setDiffuse(1.0f, 1.0f, 0.0f, 0.3f); // Yellow
                break;
                
            case GuardState::ALERTED:
            case GuardState::SEARCHING:
                material->getTechnique(0)->getPass(0)->setDiffuse(1.0f, 0.0f, 0.0f, 0.3f); // Red
                break;
        }
    }
}