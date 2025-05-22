#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreFrameListener.h>
#include <OgreBitesPrerequisites.h>
#include <OgreInput.h>
#include <OgreLogManager.h>
#include <OgreViewport.h>

class CameraController : public OgreBites::InputListener {
    public:
        CameraController(Ogre::Camera* camera, Ogre::SceneManager* sceneMgr);
        void handleMouseClick(const OgreBites::MouseButtonEvent& evt);
        //void handleMousePressed(const OgreBites::MouseButtonEvent& evt);
        //void handleMouseMoved(const OgreBites::MouseMotionEvent& evt);
        void update(const Ogre::FrameEvent& evt);

        // Méthodes pour obtenir la position et le lookAt de la caméra
        Ogre::Vector3 getCameraPosition() const { return mCameraNode->getPosition(); }
        Ogre::Vector3 getCameraLookAt() const { return mCameraTargetLookAt; }

    private:
        Ogre::Camera* mCamera;
        Ogre::SceneManager* mSceneMgr;
        Ogre::SceneNode* mCameraNode;
        bool mCameraMoving;
        bool mRotating;
        Ogre::Vector3 mCameraTargetPos;
        Ogre::Vector3 mCameraTargetLookAt;
        Ogre::Radian mYaw;
        Ogre::Radian mPitch;
        Ogre::Vector2 mLastMousePos;
};

#endif