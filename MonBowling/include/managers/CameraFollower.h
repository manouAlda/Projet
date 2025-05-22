#ifndef CAMERAFOLLOWER_H
#define CAMERAFOLLOWER_H

#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <Ogre.h>
#include "../include/objects/BowlingBall.h"

class CameraFollower {
    private:
        // Référence à la caméra
        Ogre::Camera* mCamera;
        
        // Référence au nœud de la caméra
        Ogre::SceneNode* mCameraNode;
        
        // Référence à la boule
        BowlingBall* mBall;
        
        // État du suivi
        bool mFollowing;
        
        // Position et orientation initiales de la caméra
        Ogre::Vector3 mInitialPosition;
        Ogre::Quaternion mInitialOrientation;
        
        // Position cible de la caméra
        Ogre::Vector3 mTargetPosition;
        
        // Point cible que la caméra regarde
        Ogre::Vector3 mLookAtTarget;
        
        // Vitesse de transition
        float mTransitionSpeed;
        
        // Décalage de la caméra par rapport à la boule
        Ogre::Vector3 mOffset;
        
        // Retour à la position initiale
        bool mReturningToInitial;
        float mReturnProgress;
        
    public:
        CameraFollower(Ogre::Camera* camera, BowlingBall* ball);
        ~CameraFollower();
        
        // Initialisation du suiveur de caméra
        void initialize();
        
        // Mise à jour de la position de la caméra
        void update(float deltaTime);
        
        // Démarrer le suivi de la boule
        void startFollowing();
        
        // Arrêter le suivi et revenir à la position initiale
        void stopFollowing();
        
        // Vérifier si la caméra est en train de suivre la boule
        bool isFollowing() const;
        
        // Définir la position initiale de la caméra
        void setInitialPosition(const Ogre::Vector3& position);
        
        // Définir l'orientation initiale de la caméra
        void setInitialOrientation(const Ogre::Quaternion& orientation);

};

#endif 

