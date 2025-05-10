#include "../Header/PhysicsFrameListener.hpp"

PhysicsFrameListener::PhysicsFrameListener(Physics* physics) : mPhysics(physics) {}

bool PhysicsFrameListener::frameStarted(const FrameEvent& evt) {
    // Mettre à jour la simulation physique
    mPhysics->update(evt.timeSinceLastFrame);
    return true;
}