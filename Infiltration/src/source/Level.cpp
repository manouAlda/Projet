#include "Level.h"
#include "Guard.h"
#include <fstream>
#include <sstream>
#include <iostream>

Level::Level(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world)
    : mSceneMgr(sceneMgr)
    , mDynamicsWorld(world)
    , mPlayerStartPosition(Ogre::Vector3::ZERO)
    , mObjectivePosition(Ogre::Vector3::ZERO)
    , mDebugDrawEnabled(false)
{
    // Default player start position
    mPlayerStartPosition = Ogre::Vector3(0, 1, 0);
}

Level::~Level() {
    // Clean up physics objects
    for (auto body : mRigidBodies) {
        mDynamicsWorld->removeRigidBody(body);
        delete body;
    }
    
    for (auto shape : mShapes) {
        delete shape;
    }
    
    mRigidBodies.clear();
    mShapes.clear();
}

void Level::loadLevel(const std::string& levelName) {
    // Clear any existing level data
    for (auto body : mRigidBodies) {
        mDynamicsWorld->removeRigidBody(body);
        delete body;
    }
    
    for (auto shape : mShapes) {
        delete shape;
    }
    
    mRigidBodies.clear();
    mShapes.clear();
    mLevelObjects.clear();
    mGuardStartPositions.clear();
    mGuardPatrolPaths.clear();
    
    // Load level from file
    std::string filename = levelName + ".lvl";
    parseLevelFile(filename);
    
    // Create physics objects for level geometry
    for (const auto& obj : mLevelObjects) {
        createPhysicsObject(obj);
    }
}

void Level::createSimpleLevel() {
    // Clear any existing level data
    for (auto body : mRigidBodies) {
        mDynamicsWorld->removeRigidBody(body);
        delete body;
    }
    
    for (auto shape : mShapes) {
        delete shape;
    }
    
    mRigidBodies.clear();
    mShapes.clear();
    mLevelObjects.clear();
    mGuardStartPositions.clear();
    mGuardPatrolPaths.clear();
    
    // Create a simple test level
    
    // Floor
    mLevelObjects.emplace_back(ObjectData::Type::FLOOR, Ogre::Vector3(0, -0.5f, 0), Ogre::Vector3(50, 1, 50));
    
    // Walls (creating a simple room)
    mLevelObjects.emplace_back(ObjectData::Type::WALL, Ogre::Vector3(-25, 2, 0), Ogre::Vector3(1, 5, 50));  // Left wall
    mLevelObjects.emplace_back(ObjectData::Type::WALL, Ogre::Vector3(25, 2, 0), Ogre::Vector3(1, 5, 50));   // Right wall
    mLevelObjects.emplace_back(ObjectData::Type::WALL, Ogre::Vector3(0, 2, -25), Ogre::Vector3(50, 5, 1));  // Back wall
    mLevelObjects.emplace_back(ObjectData::Type::WALL, Ogre::Vector3(0, 2, 25), Ogre::Vector3(50, 5, 1));   // Front wall
    
    // Interior walls creating a maze-like structure
    mLevelObjects.emplace_back(ObjectData::Type::WALL, Ogre::Vector3(-15, 2, 10), Ogre::Vector3(20, 5, 1));
    mLevelObjects.emplace_back(ObjectData::Type::WALL, Ogre::Vector3(10, 2, -5), Ogre::Vector3(1, 5, 30));
    mLevelObjects.emplace_back(ObjectData::Type::WALL, Ogre::Vector3(-5, 2, -15), Ogre::Vector3(20, 5, 1));
    
    // Obstacles
    mLevelObjects.emplace_back(ObjectData::Type::OBSTACLE, Ogre::Vector3(-10, 1, 0), Ogre::Vector3(2, 2, 2));
    mLevelObjects.emplace_back(ObjectData::Type::OBSTACLE, Ogre::Vector3(15, 1, 15), Ogre::Vector3(2, 2, 2));
    mLevelObjects.emplace_back(ObjectData::Type::OBSTACLE, Ogre::Vector3(0, 1, -10), Ogre::Vector3(2, 2, 2));
    
    // Player start position
    mPlayerStartPosition = Ogre::Vector3(-20, 1, -20);
    mLevelObjects.emplace_back(ObjectData::Type::PLAYER_START, mPlayerStartPosition);
    
    // Objective position
    mObjectivePosition = Ogre::Vector3(20, 1, 20);
    mLevelObjects.emplace_back(ObjectData::Type::OBJECTIVE, mObjectivePosition);
    
    // Guard start positions
    mGuardStartPositions.push_back(Ogre::Vector3(0, 1, 0));
    mGuardStartPositions.push_back(Ogre::Vector3(15, 1, -20));
    
    // Guard patrol paths
    std::vector<Ogre::Vector3> patrolPath1 = {
        Ogre::Vector3(0, 0, 0),
        Ogre::Vector3(0, 0, 15),
        Ogre::Vector3(-15, 0, 15),
        Ogre::Vector3(-15, 0, 0)
    };
    
    std::vector<Ogre::Vector3> patrolPath2 = {
        Ogre::Vector3(15, 0, -20),
        Ogre::Vector3(15, 0, -5),
        Ogre::Vector3(20, 0, -5),
        Ogre::Vector3(20, 0, -20)
    };
    
    mGuardPatrolPaths.push_back(patrolPath1);
    mGuardPatrolPaths.push_back(patrolPath2);
    
    // Create physics objects for level geometry
    for (const auto& obj : mLevelObjects) {
        createPhysicsObject(obj);
    }
}

void Level::createPhysicsObject(const ObjectData& objectData) {
    switch (objectData.type) {
        case ObjectData::Type::WALL:
            createWall(objectData.position, objectData.scale, objectData.orientation);
            break;
            
        case ObjectData::Type::FLOOR:
            createFloor(objectData.position, objectData.scale);
            break;
            
        case ObjectData::Type::OBSTACLE:
            createObstacle(objectData.position, objectData.scale, objectData.orientation);
            break;
            
        case ObjectData::Type::OBJECTIVE:
            createObjective(objectData.position);
            break;
            
        default:
            // Other types don't need physics
            break;
    }
}

void Level::createWall(const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation) {
    // Create visual entity for the wall
    Ogre::SceneNode* wallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    wallNode->setPosition(position);
    wallNode->setScale(scale);
    wallNode->setOrientation(orientation);
    
    Ogre::Entity* wallEntity = mSceneMgr->createEntity("wall_" + std::to_string(mRigidBodies.size()), "cube.mesh");
    wallEntity->setMaterialName("Wall");
    wallNode->attachObject(wallEntity);
    
    // Create physics shape for the wall
    OgreBulletCollisions::BoxCollisionShape* wallShape = new OgreBulletCollisions::BoxCollisionShape(scale);
    mShapes.push_back(wallShape);
    
    // Create rigid body for the wall
    OgreBulletDynamics::RigidBody* wallBody = new OgreBulletDynamics::RigidBody(
        "wallBody_" + std::to_string(mRigidBodies.size()),
        mDynamicsWorld,
        0,  // Collision group
        ~0  // Collision mask (collide with everything)
    );
    
    wallBody->setStaticShape(wallNode, wallShape, 0.8f, 0.1f, position, orientation);
    mRigidBodies.push_back(wallBody);
}