/*
// Guard.h - Classe pour gérer les gardes
#ifndef GUARD_H
#define GUARD_H

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <vector>

#include "OgreBulletDynamicsRigidBody.h"

class GameManager;
class Player;

class Guard
{
private:
    GameManager* mGameManager;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    OgreBulletDynamics::RigidBody* mBody;
    
    // Attributs du garde
    Ogre::String mName;
    float mSpeed;
    float mRotationSpeed;
    float mDetectionRange;
    float mFieldOfView;  // en degrés
    float mAlertLevel;   // 0.0 = normal, 1.0 = alerte maximale
    
    // État du garde
    enum GuardState {
        PATROLLING,
        INVESTIGATING,
        CHASING,
        RETURNING
    };
    
    GuardState mState;
    
    // Patrouille
    std::vector<Ogre::Vector3> mPatrolPath;
    int mCurrentPatrolIndex;
    float mWaitTime;
    float mWaitTimer;
    
    // Détection
    Ogre::SceneNode* mVisionConeNode;
    Ogre::Entity* mVisionConeEntity;
    float mTimeSinceLastSeen;
    Ogre::Vector3 mLastSeenPosition;
    
    // Méthodes internes
    void updatePatrolling(float deltaTime);
    void updateInvestigating(float deltaTime);
    void updateChasing(float deltaTime);
    void updateReturning(float deltaTime);
    void updateVisionCone();
    void createVisionCone();
    void updateVisionConeColor();
    bool isPositionInSight(const Ogre::Vector3& position) const;
    void moveToPosition(const Ogre::Vector3& position, float deltaTime);
    void rotateToFace(const Ogre::Vector3& position, float deltaTime);
    
public:
    Guard(GameManager* gameManager, const Ogre::String& name, const Ogre::Vector3& position);
    ~Guard();
    
    // Méthodes de mise à jour
    void update(float deltaTime);
    
    // Définir le chemin de patrouille
    void setPatrolPath(const std::vector<Ogre::Vector3>& path);
    
    // Détection du joueur
    bool detectPlayer(Player* player);
    
    // Accesseurs
    Ogre::Vector3 getPosition() const;
    Ogre::Quaternion getOrientation() const;
    float getAlertLevel() const { return mAlertLevel; }
    GuardState getState() const { return mState; }
};

#endif // GUARD_H
*/

#ifndef GUARD_H
#define GUARD_H

#include <Ogre.h>
#include <OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h>
#include <vector>
#include <memory>

class Level;

enum class GuardState {
    PATROLLING,
    SUSPICIOUS,
    ALERTED,
    SEARCHING
};

class Guard {
public:
    Guard(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, 
          Level* level, const Ogre::Vector3& startPosition);
    ~Guard();

    void update(float deltaTime);
    void setPatrolPath(const std::vector<Ogre::Vector3>& waypoints);
    bool canSeePlayer(const Ogre::Vector3& playerPosition) const;
    void reactToNoise(const Ogre::Vector3& noisePosition, float intensity);
    
    // Getters
    Ogre::SceneNode* getSceneNode() const { return mSceneNode; }
    GuardState getState() const { return mState; }
    float getAlertLevel() const { return mAlertLevel; }
    
private:
    // Core components
    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode* mSceneNode;
    Ogre::Entity* mEntity;
    OgreBulletDynamics::RigidBody* mBody;
    Level* mLevel;
    
    // AI state
    GuardState mState;
    float mAlertLevel;
    float mStateTime;
    
    // Vision properties
    float mVisionConeAngle;
    float mVisionRange;
    Ogre::SceneNode* mVisionConeNode;
    Ogre::Entity* mVisionConeEntity;
    
    // Movement and patrol
    float mWalkSpeed;
    float mRunSpeed;
    std::vector<Ogre::Vector3> mPatrolWaypoints;
    int mCurrentWaypoint;
    Ogre::Vector3 mTargetPosition;
    Ogre::Vector3 mLastKnownPlayerPosition;
    
    // Helper methods
    void updateMovement(float deltaTime);
    void updateVisionCone();
    void updateState(float deltaTime);
    void transitionToState(GuardState newState);
    Ogre::Vector3 calculateNextPosition(float deltaTime);
    void createVisionCone();
    void updateVisionConeColor();
};

#endif // GUARD_H