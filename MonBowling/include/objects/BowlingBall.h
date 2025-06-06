// include/objects/BowlingBall.h
#pragma once
#include <Ogre.h>
#include <OgreBullet.h>
#include "../managers/PhysicsManager.h"

class BowlingBall {
    private:
        Ogre::SceneManager* sceneMgr;
        
        // Nœud et entité
        Ogre::SceneNode* ballNode;
        Ogre::Entity* ballEntity;
        
        // Corps rigide pour la physique
        btRigidBody* ballBody;
        
        // Propriétés de la boule
        float radius;
        float mass;
        float scale= 0.015f;
        
        // État de la boule
        bool rolling;
        Ogre::Vector3 initialPosition;

        // Constante pour la limite Y
        const float STOP_Y_LIMIT = 12.0f;
        // Constante pour le seuil de vitesse d'arrêt
        const float STOP_VELOCITY_THRESHOLD = 0.05f; 
        
    public:
        BowlingBall(Ogre::SceneManager* sceneMgr, const Ogre::String& meshName = "ball.mesh");
        ~BowlingBall();
        
        // Méthodes principales
        void create(const Ogre::Vector3& position);
        void reset();
        void launch(const Ogre::Vector3& direction, float power, float spin = 0.0f);
        void update(float deltaTime);
        
        // Accesseurs
        Ogre::SceneNode* getBallNode() const { return ballNode; }
        btRigidBody* getBallBody() const { return ballBody; }
        bool isRolling() const { return rolling; }
        Ogre::Vector3 getPosition() const;
        Ogre::Vector3 getVelocity() const;
        float getRadius() const;
        
        // Modificateurs
        void setMass(float mass);
        void setFriction(float friction);
        void setRestitution(float restitution);
        void setRollingFriction(float rollingFriction);
        void setSpinningFriction(float spinningFriction);
};
