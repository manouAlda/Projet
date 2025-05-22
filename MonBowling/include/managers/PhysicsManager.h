#pragma once
#include <Ogre.h>
#include <OgreBullet.h>
#include <memory>

// Pattern Singleton pour la gestion de la physique
class PhysicsManager{
    private:
        // Instance unique (Singleton)
        static PhysicsManager* mInstance;
        
        // Constructeur privé (Singleton)
        PhysicsManager();
        
        // Monde physique Bullet
        std::unique_ptr<Ogre::Bullet::DynamicsWorld> mDynamicsWorld;
        
        // Référence au SceneManager
        Ogre::SceneManager* mSceneMgr;
        
        // Debugger visuel
        std::unique_ptr<Ogre::Bullet::DebugDrawer> mDebugDrawer;
        Ogre::SceneNode* mDebugNode;
        
    public:
        ~PhysicsManager();
        
        // Méthode d'accès à l'instance unique (Singleton)
        static PhysicsManager* getInstance();
        
        // Initialisation
        void initialize(Ogre::SceneManager* sceneMgr);
        
        // Mise à jour de la physique
        void update(float deltaTime);
        
        // Activation/désactivation du débogage visuel
        void toggleDebugDrawing();
        
        // Accesseur au monde physique
        Ogre::Bullet::DynamicsWorld* getDynamicsWorld() { return mDynamicsWorld.get(); }
};
