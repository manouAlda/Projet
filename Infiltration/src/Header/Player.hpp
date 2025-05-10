#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Ogre.h"
#include "OgreBullet.h" 
#include <memory>
#include <bullet/btBulletDynamicsCommon.h>
#include <OgreApplicationContext.h>

// Forward declarations
class btDynamicsWorld;
class btRigidBody;

class Player {
    private:
        bool checkGroundContact();
        
        Ogre::SceneManager* mSceneMgr;
        Ogre::SceneNode* mSceneNode;
        Ogre::Entity* mEntity;
        
        btRigidBody* mBody;
        
        float mMoveSpeed;
        float mJumpForce;
        bool mIsOnGround;

    public:
        Player(Ogre::SceneManager* sceneMgr, btDynamicsWorld* world, const Ogre::Vector3& position);
        ~Player();
        
        void update(float deltaTime);
        void moveLeft(float deltaTime);
        void moveRight(float deltaTime);
        void moveForward(float deltaTime);
        void moveBackward(float deltaTime);
        void jump();

        Ogre::Vector3 getPosition() const;        
        Ogre::SceneNode* getSceneNode() const;
        btRigidBody* getRigidBody() const;
        bool isOnGround() const;
};

#endif 