#ifndef MOUSE_HANDLER_HPP
#define MOUSE_HANDLER_HPP

#include <OgreApplicationContext.h>
#include <OgreCamera.h>
#include <OgreSceneNode.h>

class MouseHandler : public OgreBites::InputListener {

    private:
        Ogre::Camera* mCamera;
        Ogre::SceneNode* mCameraNode;
        bool mRightMousePressed;
        float mMoveSpeed;
        float mRotateSpeed;
        bool mLeftKeyPressed;
        bool mRightKeyPressed;
        bool mUpKeyPressed;
        bool mDownKeyPressed;

    public:
        MouseHandler(Ogre::Camera* camera, Ogre::SceneNode* cameraNode);
        ~MouseHandler() = default;

        // Mouse events
        bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
        bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
        bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
        bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) override;
        bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
        bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
        void update(float deltaTime);
    
};

#endif 