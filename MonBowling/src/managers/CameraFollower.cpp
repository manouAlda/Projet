#include "../../include/managers/CameraFollower.h"
#include <OgreLogManager.h>

CameraFollower::CameraFollower(Ogre::Camera* camera, BowlingBall* ball)
    : mCamera(camera),
      mCameraNode(nullptr),
      mBall(ball),
      mFollowing(false),
      mInitialPosition(Ogre::Vector3::ZERO),
      mInitialOrientation(Ogre::Quaternion::IDENTITY),
      mTargetPosition(Ogre::Vector3::ZERO),
      mLookAtTarget(Ogre::Vector3::ZERO),
      mTransitionSpeed(5.0f),
      mOffset(Ogre::Vector3(0, 5, 10)),  // Décalage par défaut (derrière et au-dessus)
      mReturningToInitial(false),
      mReturnProgress(0.0f) {
    
    // Récupération du nœud de la caméra
    if (mCamera) {
        mCameraNode = mCamera->getParentSceneNode();
    }
}

CameraFollower::~CameraFollower() {
}

void CameraFollower::initialize() {
    // Sauvegarde de la position et de l'orientation initiales
    if (mCameraNode) {
        mInitialPosition = mCameraNode->getPosition();
        mInitialOrientation = mCameraNode->getOrientation();
    }
}

void CameraFollower::update(float deltaTime) {
    if (!mCamera || !mCameraNode || !mBall) {
        return;
    }
    
    if (mFollowing) {
        // Obtention de la position actuelle de la boule
        Ogre::Vector3 ballPosition = mBall->getPosition();
        
        // Calcul de la position cible de la caméra (avec décalage)
        Ogre::Vector3 ballDirection = mBall->getVelocity().normalisedCopy();
        mTargetPosition = ballPosition - ballDirection * mOffset.z + Ogre::Vector3::UNIT_Y * mOffset.y;
        
        // Point que la caméra regarde (la boule)
        mLookAtTarget = ballPosition;
        
        // Interpolation fluide vers la position cible
        Ogre::Vector3 currentPos = mCameraNode->getPosition();
        Ogre::Vector3 newPos = currentPos + (mTargetPosition - currentPos) * mTransitionSpeed * deltaTime;
        mCameraNode->setPosition(newPos);
        
        // Orientation de la caméra vers la boule
        mCameraNode->lookAt(mLookAtTarget, Ogre::Node::TS_WORLD);
        
        // Vérification si la boule s'est arrêtée
        if (!mBall->isRolling() && !mReturningToInitial) {
            // Démarrer le retour à la position initiale
            mReturningToInitial = true;
            mReturnProgress = 0.0f;
        }
        
        // Gestion du retour à la position initiale
        if (mReturningToInitial) {
            mReturnProgress += deltaTime * 0.5f;  // Vitesse de retour plus lente
            
            if (mReturnProgress >= 1.0f) {
                // Fin du retour
                mCameraNode->setPosition(mInitialPosition);
                mCameraNode->setOrientation(mInitialOrientation);
                
                mFollowing = false;
                mReturningToInitial = false;
                
                Ogre::LogManager::getSingleton().logMessage("Caméra revenue à la position initiale");
            } else {
                // Interpolation entre la position actuelle et la position initiale
                Ogre::Vector3 currentPos = mCameraNode->getPosition();
                Ogre::Vector3 newPos = currentPos + (mInitialPosition - currentPos) * mReturnProgress;
                mCameraNode->setPosition(newPos);
                
                // Interpolation de l'orientation
                Ogre::Quaternion currentOrientation = mCameraNode->getOrientation();
                Ogre::Quaternion newOrientation = Ogre::Quaternion::Slerp(mReturnProgress, currentOrientation, mInitialOrientation);
                mCameraNode->setOrientation(newOrientation);
            }
        }
    }
}

void CameraFollower::startFollowing() {
    mFollowing = true;
    mReturningToInitial = false;
    mReturnProgress = 0.0f;
    
    Ogre::LogManager::getSingleton().logMessage("Caméra commence à suivre la boule");
}

void CameraFollower::stopFollowing() {
    // Démarrer le retour à la position initiale
    mReturningToInitial = true;
    mReturnProgress = 0.0f;
    
    Ogre::LogManager::getSingleton().logMessage("Caméra arrête de suivre la boule");
}

bool CameraFollower::isFollowing() const {
    return mFollowing;
}

void CameraFollower::setInitialPosition(const Ogre::Vector3& position) {
    mInitialPosition = position;
}

void CameraFollower::setInitialOrientation(const Ogre::Quaternion& orientation) {
    mInitialOrientation = orientation;
}
