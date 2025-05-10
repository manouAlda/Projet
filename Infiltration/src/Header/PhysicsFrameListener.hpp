#ifndef PHYSICSFRAMELISTENER_HPP
#define PHYSICSFRAMELISTENER_HPP

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include "Physics.hpp"

class PhysicsFrameListener : public FrameListener {
    
    private:
        Physics* mPhysics;
    
    public:
        PhysicsFrameListener(Physics* physics);
        bool frameStarted(const FrameEvent& evt) override;
};

#endif