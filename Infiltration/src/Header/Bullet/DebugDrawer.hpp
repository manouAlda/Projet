#pragma once

#include <Ogre.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/LinearMath/btIDebugDraw.h>
#include <memory>

namespace Bullet {

class DebugDrawer : public btIDebugDraw {
private:
    Ogre::SceneNode* mNode;
    btDynamicsWorld* mWorld;
    Ogre::ManualObject* mLines;
    int mDebugMode;

public:
    DebugDrawer(Ogre::SceneNode* node, btDynamicsWorld* world);
    ~DebugDrawer();

    // Méthodes de l'interface btIDebugDraw
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, 
                                btScalar distance, int lifeTime, const btVector3& color) override;
    virtual void reportErrorWarning(const char* warningString) override;
    virtual void draw3dText(const btVector3& location, const char* textString) override;
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    // Mise à jour du dessin de debug
    void update();
};

} // namespace Bullet