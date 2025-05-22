#pragma once
#include <Ogre.h>
#include "../../include/managers/PhysicsManager.h"

// Pattern Factory pour la création d'objets
class ObjectFactory{
    private:
        Ogre::SceneManager* mSceneMgr;
        
    public:
        ObjectFactory(Ogre::SceneManager* sceneMgr) : mSceneMgr(sceneMgr) {}
        
        // Méthodes de création d'objets
        btRigidBody* createStaticGround();
        btRigidBody* createDynamicBox(const Ogre::Vector3& position, const Ogre::String& name, float mass = 1.0f);
        btRigidBody* createDynamicSphere(const Ogre::Vector3& position, const Ogre::String& name, float mass = 1.0f);
        btRigidBody* createDynamicCylinder(const Ogre::Vector3& position, const Ogre::String& name, float mass = 1.0f);
        
        // Méthode pour créer une pile d'objets
        void createStack(int height, const Ogre::Vector3& position);
};
