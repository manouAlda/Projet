#include "../Header/MouseHandler.hpp"

MouseHandler::MouseHandler(Ogre::Camera* camera, Ogre::SceneNode* cameraNode)
    : mCamera(camera),
      mCameraNode(cameraNode),
      mRightMousePressed(false),
      mLeftKeyPressed(false),
      mRightKeyPressed(false),
      mUpKeyPressed(false),
      mDownKeyPressed(false),
      mMoveSpeed(50.0f),
      mRotateSpeed(0.1f) {
}

bool MouseHandler::mouseMoved(const OgreBites::MouseMotionEvent& evt) {
    if (mRightMousePressed) {
        // Rotate camera based on mouse movement
        mCameraNode->yaw(Ogre::Degree(-evt.xrel * mRotateSpeed));
        mCameraNode->pitch(Ogre::Degree(-evt.yrel * mRotateSpeed));
    }
    return true;
}

bool MouseHandler::mousePressed(const OgreBites::MouseButtonEvent& evt) {
    if (evt.button == OgreBites::BUTTON_RIGHT) {
        mRightMousePressed = true;
    }
    return true;
}

bool MouseHandler::mouseReleased(const OgreBites::MouseButtonEvent& evt) {
    if (evt.button == OgreBites::BUTTON_RIGHT) {
        mRightMousePressed = false;
    }
    return true;
}

bool MouseHandler::keyPressed(const OgreBites::KeyboardEvent& evt) {
    if (evt.keysym.sym == OgreBites::SDLK_LEFT) {
        mLeftKeyPressed = true;
    }
    else if (evt.keysym.sym == OgreBites::SDLK_RIGHT) {
        mRightKeyPressed = true;
    }
    else if (evt.keysym.sym == OgreBites::SDLK_UP) {
        mUpKeyPressed = true;
    }
    else if (evt.keysym.sym == OgreBites::SDLK_DOWN) {
        mDownKeyPressed = true;
    }
    return true;
}

bool MouseHandler::keyReleased(const OgreBites::KeyboardEvent& evt) {
    if (evt.keysym.sym == OgreBites::SDLK_LEFT) {
        mLeftKeyPressed = false;
    }
    else if (evt.keysym.sym == OgreBites::SDLK_RIGHT) {
        mRightKeyPressed = false;
    }
    else if (evt.keysym.sym == OgreBites::SDLK_UP) {
        mUpKeyPressed = false;
    }
    else if (evt.keysym.sym == OgreBites::SDLK_DOWN) {
        mDownKeyPressed = false;
    }
    return true;
}

bool MouseHandler::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) {
    // Move camera forward/backward with mouse wheel
    Ogre::Vector3 moveDir = mCamera->getRealDirection() * (evt.y * mMoveSpeed);
    mCameraNode->translate(moveDir, Ogre::Node::TS_LOCAL);
    return true;
}

void MouseHandler::update(float deltaTime) {
    if (mLeftKeyPressed) {
        mCameraNode->yaw(Ogre::Degree(100.0f * deltaTime)); // 100 degrés/seconde
    }
    if (mRightKeyPressed) {
        mCameraNode->yaw(Ogre::Degree(-100.0f * deltaTime));
    }
    if (mUpKeyPressed) {
        mCameraNode->pitch(Ogre::Degree(100.0f * deltaTime)); // 100 degrés/seconde
    }
    if (mDownKeyPressed) {
        mCameraNode->pitch(Ogre::Degree(-100.0f * deltaTime));
    }
}