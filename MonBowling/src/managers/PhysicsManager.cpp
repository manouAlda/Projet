#include "../../include/managers/PhysicsManager.h"

// Initialisation de l'instance statique à nullptr
PhysicsManager* PhysicsManager::mInstance = nullptr;

PhysicsManager* PhysicsManager::getInstance(){
    // Création de l'instance si elle n'existe pas encore
    if (mInstance == nullptr){
        mInstance = new PhysicsManager();
    }
    return mInstance;
}

PhysicsManager::PhysicsManager()
    : mSceneMgr(nullptr),
      mDebugNode(nullptr)
{}

PhysicsManager::~PhysicsManager(){}

void PhysicsManager::initialize(Ogre::SceneManager* sceneMgr){
    mSceneMgr = sceneMgr;
    
    // Création du monde physique avec gravité (0, -9.81, 0)
    mDynamicsWorld = std::make_unique<Ogre::Bullet::DynamicsWorld>(Ogre::Vector3(0, -9.81, 0));
    
    // Configuration du debugger visuel
    mDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BulletDebugNode");
    
    // Correction: Passer à la fois le nœud de scène et le monde de collision au DebugDrawer
    mDebugDrawer = std::make_unique<Ogre::Bullet::DebugDrawer>(mDebugNode, mDynamicsWorld->getBtWorld());
    
    // Activation du débogage visuel par défaut
    mDebugDrawer->setDebugMode(Ogre::Bullet::DebugDrawer::DBG_DrawWireframe | 
                              Ogre::Bullet::DebugDrawer::DBG_DrawContactPoints);
    
    // Enregistrer le debug drawer avec le monde physique
    mDynamicsWorld->getBtWorld()->setDebugDrawer(mDebugDrawer.get());
}

void PhysicsManager::update(float deltaTime){
    // Mise à jour de la simulation physique
    mDynamicsWorld->getBtWorld()->stepSimulation(deltaTime, 10);
    
    // Mise à jour du debugger visuel
    if (mDebugDrawer && mDebugDrawer->getDebugMode() > 0){
        mDebugDrawer->update();
    }
}

void PhysicsManager::toggleDebugDrawing(){
    if (mDebugDrawer){
        int mode = mDebugDrawer->getDebugMode();
        if (mode > 0){
            // Désactivation du débogage visuel
            mDebugDrawer->setDebugMode(0);
        }
        else{
            // Activation du débogage visuel
            mDebugDrawer->setDebugMode(Ogre::Bullet::DebugDrawer::DBG_DrawWireframe | 
                                      Ogre::Bullet::DebugDrawer::DBG_DrawContactPoints);
        }
    }
}