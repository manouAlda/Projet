#ifndef CAMERAFOLLOWER_H
#define CAMERAFOLLOWER_H

#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <Ogre.h>
#include <OgreQuaternion.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include "../include/objects/BowlingBall.h"

class CameraFollower {
    private:
        // Référence à la caméra
        Ogre::Camera* camera;
        Ogre::SceneNode* cameraNode;
        BowlingBall* ball;
        bool following;
        
        // Position et orientation initiales de la caméra
        Ogre::Vector3 initialPosition;
        // Position cible de la caméra
        Ogre::Vector3 targetPosition;
        // Point cible que la caméra regarde
        Ogre::Vector3 lookAtTarget;
        // Vitesse de transition
        float transitionSpeed;
        
        Ogre::Quaternion initialOrientation; 
        Ogre::Vector3 pinsLookAt;      
        float postRollTimer;                 
        Ogre::Vector3 returnStartPosition;   
        Ogre::Quaternion returnStartOrientation; 

        // Enumération pour les états post-lancer
        enum class PostRollState {
            NONE,
            FOCUS_BALL,
            TRANSITION_TO_PINS,
            FOCUS_PINS,
            RETURNING
        };
        PostRollState postRollState; 
            
        // Décalage de la caméra par rapport à la boule
        Ogre::Vector3 offset;
        
        // Retour à la position initiale
        bool returningToInitial;
        float returnProgress;
        Ogre::Vector3 transitionStart;
        Ogre::Vector3 transitionEnd;

        const float FOCUS_BALL_DURATION = 0.25f;
        const float TRANSITION_DURATION = 1.0f;
        const float FOCUS_PINS_DURATION = 2.0f;
        const float RETURN_SPEED = 1.5f; 
        const float PINS_Z_THRESHOLD = -10.0f;
        const float STOP_DISTANCE = 3.0f;

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

        void resetToStartPosition();

        bool isSequenceActive() const; 
        void setInitialOrientation(const Ogre::Quaternion& orientation); 
};

#endif 
