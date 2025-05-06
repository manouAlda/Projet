#ifndef LEVEL_H
#define LEVEL_H

#include <Ogre.h>
#include <OgreBullet/Dynamics/OgreBulletDynamicsRigidBody.h>
#include <OgreBullet/Collisions/OgreBulletCollisionsShape.h>
#include <OgreBullet/Dynamics/OgreBulletDynamicsWorld.h>
#include <OgreBullet/Collisions/OgreBulletCollisionsRay.h>
#include <vector>
#include <memory>
#include <utility>

class Guard;

struct ObjectData {
    enum class Type {
        WALL,
        FLOOR,
        OBSTACLE,
        OBJECTIVE,
        PLAYER_START,
        GUARD_PATROL_POINT
    };
    
    Type type;
    Ogre::Vector3 position;
    Ogre::Vector3 scale;
    Ogre::Quaternion orientation;
    
    ObjectData(Type t, const Ogre::Vector3& pos, const Ogre::Vector3& sc = Ogre::Vector3::UNIT_SCALE, 
               const Ogre::Quaternion& orient = Ogre::Quaternion::IDENTITY)
        : type(t), position(pos), scale(sc), orientation(orient) {}
};

class Level {
public:
    Level(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world);
    ~Level();
    
    void loadLevel(const std::string& levelName);
    void createSimpleLevel();
    
    std::pair<bool, float> raycastToPoint(const Ogre::Ray& ray, float maxDistance) const;
    
    const Ogre::Vector3& getPlayerStartPosition() const { return mPlayerStartPosition; }
    const Ogre::Vector3& getObjectivePosition() const { return mObjectivePosition; }
    const std::vector<std::vector<Ogre::Vector3>>& getGuardPatrolPaths() const { return mGuardPatrolPaths; }
    const std::vector<Ogre::Vector3>& getGuardStartPositions() const { return mGuardStartPositions; }
    bool isPlayerAtObjective(const Ogre::Vector3& playerPosition) const;
    
    // For debugging
    void toggleDebugRendering();
    
private:
    Ogre::SceneManager* mSceneMgr;
    OgreBulletDynamics::DynamicsWorld* mDynamicsWorld;
    
    // Level data
    std::vector<ObjectData> mLevelObjects;
    Ogre::Vector3 mPlayerStartPosition;
    Ogre::Vector3 mObjectivePosition;
    std::vector<Ogre::Vector3> mGuardStartPositions;
    std::vector<std::vector<Ogre::Vector3>> mGuardPatrolPaths;
    
    // Physics objects
    std::vector<OgreBulletDynamics::RigidBody*> mRigidBodies;
    std::vector<OgreBulletCollisions::CollisionShape*> mShapes;
    
    // Helper methods
    void createPhysicsObject(const ObjectData& objectData);
    void createWall(const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation);
    void createFloor(const Ogre::Vector3& position, const Ogre::Vector3& scale);
    void createObstacle(const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation);
    void createObjective(const Ogre::Vector3& position);
    void parseLevelFile(const std::string& filename);
    
    // Debug
    bool mDebugDrawEnabled;
};

#endif // LEVEL_H