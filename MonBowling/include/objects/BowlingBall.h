// include/objects/BowlingBall.h
#pragma once
#include <Ogre.h>
#include <OgreBullet.h>
#include "../managers/PhysicsManager.h"

class BowlingBall {
    private:
        Ogre::SceneManager* mSceneMgr;
        
        // Nœud et entité
        Ogre::SceneNode* mBallNode;
        Ogre::Entity* mBallEntity;
        
        // Corps rigide pour la physique
        btRigidBody* mBallBody;
        
        // Propriétés de la boule
        float mRadius;
        float mMass;
        float scale= 4.0f;
        Ogre::ColourValue mColor;
        
        // État de la boule
        bool mIsRolling;
        Ogre::Vector3 mInitialPosition;
        
    public:
        BowlingBall(Ogre::SceneManager* sceneMgr, const Ogre::String& meshName = "ball.mesh");
        ~BowlingBall();
        
        // Méthodes principales
        void create(const Ogre::Vector3& position);
        void reset();
        void launch(const Ogre::Vector3& direction, float power, float spin = 0.0f);
        void update(float deltaTime);
        
        // Accesseurs
        Ogre::SceneNode* getBallNode() const { return mBallNode; }
        btRigidBody* getBallBody() const { return mBallBody; }
        bool isRolling() const { return mIsRolling; }
        Ogre::Vector3 getPosition() const;
        Ogre::Vector3 getVelocity() const;
        
        // Modificateurs
        void setColor(const Ogre::ColourValue& color);
        void setMass(float mass);
        void setFriction(float friction);
        void setRestitution(float restitution);
        void setRollingFriction(float rollingFriction);
        void setSpinningFriction(float spinningFriction);
};
