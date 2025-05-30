#include "../../include/managers/CameraFollower.h"

CameraFollower::CameraFollower(Ogre::Camera* camera, BowlingBall* ball)
    : mCamera(camera),
      mCameraNode(nullptr),
      mBall(ball),
      mFollowing(false),
      mInitialPosition(Ogre::Vector3::ZERO),
      mTargetPosition(Ogre::Vector3::ZERO),
      mLookAtTarget(Ogre::Vector3::ZERO),
      mTransitionSpeed(5.0f),
      mOffset(Ogre::Vector3(0.0f, 2.0f, 1.5f)),  
      mReturningToInitial(false),
      mReturnProgress(0.0f) {
    
    // Récupération du nœud de la caméra
    if (mCamera) {
        mCameraNode = mCamera->getParentSceneNode();
    }
}

CameraFollower::~CameraFollower() {}

void CameraFollower::initialize() {
    if (mCameraNode) {
        mInitialPosition = mCameraNode->getPosition();
    }
}

void CameraFollower::update(float deltaTime) {
    if (!mCamera || !mCameraNode || !mBall) {
        return;
    }
    
    if (mFollowing) {
        Ogre::Vector3 ballPosition = mBall->getPosition();
        
        // Calcul de la position cible de la caméra avec hauteur fixe
        // Conserver uniquement le x et z de la boule, avec un y fixe à 10
        mTargetPosition = Ogre::Vector3(
            ballPosition.x,                
            0.5f,                         
            ballPosition.z - mOffset.z     
        );
        
        // Point que la caméra regarde (un quille)
        const float LOOKAT_CHANGE_THRESHOLD_Z = 5.0f;
        if (ballPosition.z > LOOKAT_CHANGE_THRESHOLD_Z) {
            mLookAtTarget = ballPosition; 
        } else {
            mLookAtTarget = Ogre::Vector3(-3, 2, 80);
        }
        
        // Interpolation fluide vers la position cible
        Ogre::Vector3 currentPos = mCameraNode->getPosition();
        Ogre::Vector3 newPos = currentPos + (mTargetPosition - currentPos) * mTransitionSpeed * deltaTime;
        mCameraNode->setPosition(newPos);
        
        mCameraNode->lookAt(mLookAtTarget, Ogre::Node::TS_WORLD);
        
        // Vérification si la boule s'est arrêtée
        if (!mBall->isRolling() && !mReturningToInitial) {
            mReturningToInitial = true;
            mReturnProgress = 0.0f;
        }

        if (mReturningToInitial) {
            mReturnProgress += deltaTime * 0.5f;  // Vitesse de retour plus lente
            
            if (mReturnProgress >= 1.0f) {
                mCameraNode->setPosition(mInitialPosition);
                
                mFollowing = false;
                mReturningToInitial = false;
                
                Ogre::LogManager::getSingleton().logMessage("Caméra revenue à la position initiale");
            } else {
                Ogre::Vector3 currentPos = mCameraNode->getPosition();
                Ogre::Vector3 newPos = currentPos + (mInitialPosition - currentPos) * mReturnProgress;
                mCameraNode->setPosition(newPos);
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

void CameraFollower::resetToStartPosition() {
    if (mCameraNode) {
        // Position et orientation de départ
        Ogre::Vector3 startPosition = Ogre::Vector3(0.0f, 1.0f, -11.0f);
        //Ogre::Vector3 startPosition = Ogre::Vector3(15, 5, -85);
        // Ogre::Quaternion startOrientation = Ogre::Quaternion(Ogre::Degree(-10), Ogre::Vector3::UNIT_X); // Regarde légèrement vers le bas

        // Définir la position et l'orientation via le nœud de scène
        mCameraNode->setPosition(startPosition);
        // mCameraNode->setOrientation(startOrientation);

        mFollowing = false; // Réinitialiser l'état de suivi

        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Caméra réinitialisée à la position de départ:"+Ogre::StringConverter::toString(startPosition));
    } else {
        Ogre::LogManager::getSingleton().logMessage("ERREUR: CameraFollower - La caméra n'est attachée à aucun SceneNode.");
    }
}
