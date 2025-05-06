// Player.h - Classe pour gérer le joueur
#ifndef PLAYER_H
#define PLAYER_H

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <OIS/OIS.h>

#include "OgreBulletDynamicsRigidBody.h"

class GameManager;

class Player
{
private:
    GameManager* mGameManager;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    OgreBulletDynamics::RigidBody* mBody;
    
    // Attributs du joueur
    Ogre::String mName;
    float mWalkSpeed;
    float mRunSpeed;
    float mCurrentSpeed;
    float mTurnSpeed;
    float mJumpForce;
    float mStealthLevel;
    bool mIsRunning;
    bool mIsVisible;
    bool mIsOnGround;
    
    // Contrôles du joueur
    bool mMoveForward;
    bool mMoveBackward;
    bool mMoveLeft;
    bool mMoveRight;
    bool mJump;
    bool mIsCrouching;
    
    // Caméra du joueur
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mCameraNode;
    float mCameraHeight;
    float mCameraPitch;
    float mCameraYaw;
    
    // État du joueur
    enum PlayerState {
        IDLE,
        WALKING,
        RUNNING,
        CROUCHING,
        JUMPING
    };
    
    PlayerState mState;
    
    // Méthodes internes
    void updateMovement(float deltaTime);
    void updateCamera(float deltaTime);
    void updateState();
    void createPhysicsBody();
    
public:
    Player(GameManager* gameManager, const Ogre::String& name, const Ogre::Vector3& position);
    ~Player();
    
    // Méthodes de mise à jour
    void update(float deltaTime);
    
    // Gestion des entrées
    void keyPressed(const OIS::KeyEvent& evt);
    void keyReleased(const OIS::KeyEvent& evt);
    void mouseMoved(const OIS::MouseEvent& evt);
    void mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
    void mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
    
    // Actions du joueur
    void move(const Ogre::Vector3& direction, float deltaTime);
    void rotate(float yaw, float pitch);
    void jump();
    void crouch(bool isCrouching);
    void run(bool isRunning);
    void interact();
    
    // Attachement de la caméra
    void attachCamera(Ogre::Camera* camera);
    
    // Accesseurs
    Ogre::Vector3 getPosition() const;
    Ogre::Quaternion getOrientation() const;
    float getStealthLevel() const { return mStealthLevel; }
    bool isRunning() const { return mIsRunning; }
    bool isVisible() const { return mIsVisible; }
    
    // Pour la détection
    Ogre::SceneNode* getSceneNode() const { return mNode; }
};

#endif // PLAYER_H