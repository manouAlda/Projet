#pragma once

#include <Ogre.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <memory>
#include <map>

namespace Bullet {

// Types de collision supportés
enum CollisionType {
    CT_BOX,
    CT_SPHERE,
    CT_CYLINDER,
    CT_CAPSULE,
    CT_CONE,
    CT_TRIMESH,
    CT_HULL
};

class DynamicsWorld {
private:
    // Pointeur unique vers le monde physique Bullet
    std::unique_ptr<btDiscreteDynamicsWorld> mWorld;
    // Autres composants de Bullet
    std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfig;
    std::unique_ptr<btCollisionDispatcher> mDispatcher;
    std::unique_ptr<btBroadphaseInterface> mBroadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> mSolver;

    // Map pour stocker les corps rigides
    std::map<Ogre::MovableObject*, btRigidBody*> mRigidBodies;

public:
    // Constructeur prenant une gravité
    DynamicsWorld(const Ogre::Vector3& gravity = Ogre::Vector3(0, -9.81, 0));
    
    // Destructeur
    ~DynamicsWorld();

    // Ajouter un corps rigide au monde
    btRigidBody* addRigidBody(float mass, Ogre::Entity* entity, CollisionType type);
    
    // Effectuer un pas de simulation
    void stepSimulation(float timeStep, int maxSubSteps = 1);
    
    // Supprimer un corps rigide du monde
    void removeRigidBody(btRigidBody* body);
    
    // Supprimer un corps rigide associé à une entité
    void removeRigidBody(Ogre::MovableObject* movable);
    
    // Accéder au monde Bullet
    btDiscreteDynamicsWorld* getBtWorld() { return mWorld.get(); }
};

} 