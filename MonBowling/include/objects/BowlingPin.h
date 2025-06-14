#ifndef BOWLING_PIN_H
#define BOWLING_PIN_H

#include <Ogre.h>
#include "../../include/managers/PhysicsManager.h"

class BowlingPin {

    private:
        Ogre::SceneManager* sceneMgr;
        Ogre::SceneNode* pinNode;
        Ogre::Entity* pinEntity;
        btRigidBody* pinBody;
        Ogre::Vector3 initialPosition;
        
    public:
        BowlingPin(Ogre::SceneManager* sceneMgr);
        ~BowlingPin();
        
        void create(const Ogre::Vector3& position, int pinIndex);
        void reset();
        void update(float deltaTime);
        bool isKnockedDown() const;
        
        btRigidBody* getPinBody() const;
        Ogre::SceneNode* getPinNode() const;
        
};

#endif 