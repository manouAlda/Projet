#pragma once

#include <Ogre.h>
#include <memory>
#include <bullet/btBulletDynamicsCommon.h>
#include "Bullet/DynamicsWorld.hpp"
#include "Bullet/DebugDrawer.hpp"
#include "Plane.hpp"

using namespace Ogre;

class Physics {
private:
    Ogre::SceneManager* sceneMgr;
    std::unique_ptr<Bullet::DynamicsWorld> world;
    ::Plane* mPlane;
    std::unique_ptr<Bullet::DebugDrawer> debugDrawer;
    Ogre::SceneNode* debugNode;

public:
    Physics(Ogre::SceneManager* sceneMgr);
    ~Physics();

    void update(float deltaTime);
    Bullet::DynamicsWorld* getDynamicsWorld();
    
    // Méthode pour accéder directement au monde Bullet
    btDiscreteDynamicsWorld* getBtWorld() {
        return world->getBtWorld();
    }

    void createStaticGround(Physics* physics, SceneManager* sceneMgr);
    btRigidBody* createDynamicCube(Physics* physics, Ogre::SceneManager* sceneMgr, const Ogre::Vector3& position, const Ogre::String& name, float mass = 1.0f);
    btRigidBody* createDynamicSphere(Physics* physics, Ogre::SceneManager* sceneMgr, const Ogre::Vector3& position, const Ogre::String& name, float mass = 1.0f);
    btRigidBody* createDynamicCharacter(Physics* physics, Ogre::SceneManager* sceneMgr, const Ogre::Vector3& position, const Ogre::String& name, float mass = 80.0f);
};