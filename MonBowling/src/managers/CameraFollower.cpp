#include "../../include/managers/CameraFollower.h"
#include <OgreMath.h>

CameraFollower::CameraFollower(Ogre::Camera* camera, BowlingBall* ball)
    : camera(camera),
      cameraNode(nullptr),
      ball(ball),
      following(false),
      initialPosition(0.0f, 1.5f, 12.5f),
      initialOrientation(Ogre::Quaternion::IDENTITY),
      pinsLookAt(0.0f, 0.5f, -10.0f),
      targetPosition(Ogre::Vector3::ZERO),
      lookAtTarget(Ogre::Vector3::ZERO),
      transitionSpeed(4.0f),
      offset(0.0f, 1.5f, 3.0f),
      postRollState(PostRollState::NONE),
      postRollTimer(0.0f),
      returnProgress(0.0f),
      returnStartPosition(Ogre::Vector3::ZERO),
      returnStartOrientation(Ogre::Quaternion::IDENTITY)
{
    if (camera) {
        cameraNode = camera->getParentSceneNode();
        if (cameraNode) {
            cameraNode->setPosition(initialPosition);
            cameraNode->lookAt(pinsLookAt, Ogre::Node::TS_WORLD); 
            initialOrientation = cameraNode->getOrientation();
            Ogre::LogManager::getSingleton().logMessage("CameraFollower: Orientation initiale définie.");
        }
    }
}

CameraFollower::~CameraFollower() {}

void CameraFollower::initialize() {
    resetToStartPosition(); 
    Ogre::LogManager::getSingleton().logMessage("CameraFollower initialisé.");
}

void CameraFollower::update(float deltaTime) {
    if (!camera || !cameraNode || !ball) {
        return;
    }
    
    if (following) {
        Ogre::Vector3 ballPosition = ball->getPosition();

        if (ballPosition.z <= PINS_Z_THRESHOLD) {
            // Arrêter la caméra à une distance fixe derrière la boule
            Ogre::Vector3 stopPosition = ballPosition - Ogre::Vector3::NEGATIVE_UNIT_Z * STOP_DISTANCE;
            stopPosition.y = std::max(stopPosition.y, 0.8f); // Maintenir une hauteur minimale
            cameraNode->setPosition(stopPosition);
            //cameraNode->lookAt(Ogre::Vector3(0,0.5,-8) , Ogre::Node::TS_WORLD);
            Ogre::LogManager::getSingleton().logMessage("CameraFollower: Boule atteignant quilles, caméra arrêtée à: " + 
                Ogre::StringConverter::toString(stopPosition));

            // Passer directement à FOCUS_PINS
            following = false;
            postRollState = PostRollState::TRANSITION_TO_PINS;
            postRollTimer = 0.0f;
            lookAtTarget = pinsLookAt;
        } else {
            Ogre::Vector3 ballVelocity = ball->getVelocity();
            float ballSpeed = ballVelocity.length();
            
            // BOWLING: Position de la caméra selon la phase de jeu
            Ogre::Vector3 cameraOffset;

            if (ballSpeed > 0.5f) {
                // PHASE ACTIVE: La boule roule vers les quilles
                // Caméra derrière la boule, légèrement surélevée
                Ogre::Vector3 ballDirection = ballVelocity.normalisedCopy();
                
                // Distance adaptative selon la vitesse
                float dynamicDistance = offset.z + (ballSpeed * 0.3f); // Plus rapide = plus loin
                dynamicDistance = std::min(dynamicDistance, offset.z * 2.0f); // Limite max
                
                cameraOffset = -ballDirection * dynamicDistance + // Derrière la boule
                            Ogre::Vector3::UNIT_Y * offset.y + // Hauteur
                            Ogre::Vector3::UNIT_X * offset.x; // Décalage latéral si nécessaire
            } else {
                // PHASE LENTE/ARRÊT: Caméra plus proche et centrée
                cameraOffset = Ogre::Vector3(0, offset.y * 0.8f, offset.z * 0.6f);
            }

            targetPosition = ballPosition + cameraOffset;
            // Hauteur minimum pour éviter le sol
            targetPosition.y = std::max(targetPosition.y, 0.8f);
            
            // BOWLING: Cible de regard adaptée
            if (ballSpeed > 1.0f) {
                // Boule rapide: regarde légèrement devant pour anticiper
                lookAtTarget = ballPosition + ballVelocity.normalisedCopy() * 1.0f;
            } else if (ballSpeed > 0.1f) {
                // Boule lente: regarde la boule directement
                lookAtTarget = ballPosition;
            } else {
                // Boule arrêtée: regarde vers les quilles pour voir les résultats
                lookAtTarget = Ogre::Vector3::ZERO; // Centre des quilles
            }

            // Mouvement de caméra plus fluide pour le bowling
            Ogre::Vector3 currentPos = cameraNode->getPosition();
            float adaptiveSpeed = transitionSpeed;
            
            // Ralentir la caméra quand la boule ralentit pour un effet cinématique
            if (ballSpeed < 2.0f) {
                adaptiveSpeed *= (0.5f + ballSpeed * 0.25f);
            }
            
            Ogre::Vector3 newPos = currentPos + (targetPosition - currentPos) * (adaptiveSpeed * deltaTime);
            cameraNode->setPosition(newPos);
            
            // Rotation de caméra plus douce
            Ogre::Vector3 currentLookAt = cameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
            Ogre::Vector3 desiredLookDir = (lookAtTarget - newPos).normalisedCopy();
            Ogre::Vector3 smoothLookDir = currentLookAt + (desiredLookDir - currentLookAt) * (adaptiveSpeed * deltaTime * 0.7f);
            
            cameraNode->setDirection(smoothLookDir, Ogre::Node::TS_WORLD);

            if (!ball->isRolling()) {
                Ogre::LogManager::getSingleton().logMessage("CameraFollower: Boule arrêtée. Début séquence post-lancer.");
                following = false;
                postRollState = PostRollState::FOCUS_BALL;
                postRollTimer = 0.0f;
                lookAtTarget = ball->getPosition();
                Ogre::Vector3 ballPos = ball->getPosition();
                Ogre::LogManager::getSingleton().logMessage("CameraFollower: Boule arrêtée. Position: (" + 
                    Ogre::StringConverter::toString(ballPos.x) + ", " + 
                    Ogre::StringConverter::toString(ballPos.y) + ", " + 
                    Ogre::StringConverter::toString(ballPos.z) + ")");
            }
        }
    } else if (postRollState != PostRollState::NONE) {
        postRollTimer += deltaTime;

        switch (postRollState) {
            case PostRollState::FOCUS_BALL: {
                // Focus sur la boule arrêtée
                Ogre::Vector3 adjustedLookAt = lookAtTarget;
                adjustedLookAt.y = std::max(adjustedLookAt.y, 0.2f); // Au niveau de la boule
                adjustedLookAt.y = std::min(adjustedLookAt.y, 1.0f); // Pas trop haut
                cameraNode->lookAt(adjustedLookAt, Ogre::Node::TS_WORLD);

                if (postRollTimer >= FOCUS_BALL_DURATION) {
                    // Début de la transition vers FOCUS_PINS
                    postRollState = PostRollState::TRANSITION_TO_PINS;
                    postRollTimer = 0.0f;
                    transitionStart = adjustedLookAt; // Position de départ (boule)
                    transitionEnd = pinsLookAt; // Position d'arrivée (quilles)
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Début transition vers quilles.");
                }
                break;
            }

            case PostRollState::TRANSITION_TO_PINS: {
                // Transition fluide entre la boule et les quilles
                float t = std::min(postRollTimer / TRANSITION_DURATION, 1.0f);
                // Utiliser une courbe d'easing (smoothstep) pour plus de naturel
                float easedT = t * t * (3.0f - 2.0f * t);
                Ogre::Vector3 interpolatedLookAt = transitionStart + (transitionEnd - transitionStart) * easedT;
                cameraNode->lookAt(interpolatedLookAt, Ogre::Node::TS_WORLD);

                if (postRollTimer >= TRANSITION_DURATION) {
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Fin transition, début focus quilles.");
                    postRollState = PostRollState::FOCUS_PINS;
                    postRollTimer = 0.0f;
                }
                break;
            }

            case PostRollState::FOCUS_PINS: {
                // Focus sur les quilles
                cameraNode->lookAt(pinsLookAt, Ogre::Node::TS_WORLD);

                if (postRollTimer >= FOCUS_PINS_DURATION) {
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Fin focus quilles, début retour.");
                    postRollState = PostRollState::RETURNING;
                    postRollTimer = 0.0f;
                    returnProgress = 0.0f;
                    returnStartPosition = cameraNode->getPosition();
                    returnStartOrientation = cameraNode->getOrientation();
                }
                break;
            }

            case PostRollState::RETURNING: {
                // BOWLING: Retour fluide à la position de lancer
                returnProgress += deltaTime * RETURN_SPEED;
                returnProgress = std::min(returnProgress, 1.0f);
                
                // Courbe d'easing pour un retour plus naturel
                float easedProgress = returnProgress * returnProgress * (3.0f - 2.0f * returnProgress); // Smoothstep

                cameraNode->setPosition(returnStartPosition + (initialPosition - returnStartPosition) * easedProgress);
                cameraNode->setOrientation(Ogre::Quaternion::Slerp(easedProgress, returnStartOrientation, initialOrientation, true));

                if (returnProgress >= 1.0f) {
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Retour à la position initiale terminé.");
                    postRollState = PostRollState::NONE;
                    cameraNode->setPosition(initialPosition);
                    cameraNode->setOrientation(initialOrientation);
                }
                break;
            }

            case PostRollState::NONE:
                break;
        }
    }
}

void CameraFollower::startFollowing() {
    if (!ball || ball->isRolling()) { 
        following = true;
        postRollState = PostRollState::NONE; // Arrête toute séquence post-lancer en cours
        postRollTimer = 0.0f;
        returnProgress = 0.0f;
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Commence à suivre la boule.");
    } else {
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Tentative de suivi mais la boule ne roule pas.");
    }
}

void CameraFollower::stopFollowing() {
     if (following) {
         Ogre::LogManager::getSingleton().logMessage("CameraFollower: Arrêt manuel du suivi (stopFollowing appelé).");
         following = false;
         // Démarre la séquence post-lancer si elle n'est pas déjà active
         if (postRollState == PostRollState::NONE) {
             postRollState = PostRollState::FOCUS_BALL;
             postRollTimer = 0.0f;
             lookAtTarget = ball ? ball->getPosition() : Ogre::Vector3::ZERO;
             Ogre::LogManager::getSingleton().logMessage("CameraFollower: Début séquence post-lancer (via stopFollowing).");
         }
     }
}

bool CameraFollower::isFollowing() const {
    return following;
}

// Indique si la caméra est dans une des phases post-lancer (Focus ou Retour)
bool CameraFollower::isSequenceActive() const {
    return postRollState != PostRollState::NONE;
}

void CameraFollower::setInitialPosition(const Ogre::Vector3& position) {
    initialPosition = position;
    if (cameraNode && postRollState == PostRollState::NONE && !following) {
        cameraNode->setPosition(initialPosition);
        cameraNode->lookAt(Ogre::Vector3(0, 0.5, 0), Ogre::Node::TS_WORLD);
        initialOrientation = cameraNode->getOrientation();
    }
}

void CameraFollower::setInitialOrientation(const Ogre::Quaternion& orientation) {
    initialOrientation = orientation;
    if (cameraNode && postRollState == PostRollState::NONE && !following) {
        cameraNode->setOrientation(initialOrientation);
    }
}

void CameraFollower::resetToStartPosition() {
    if (cameraNode) {
        cameraNode->setPosition(initialPosition);
        cameraNode->lookAt(pinsLookAt, Ogre::Node::TS_WORLD); // Changement : pointe vers les quilles
        initialOrientation = cameraNode->getOrientation();
        following = false;
        postRollState = PostRollState::NONE;
        postRollTimer = 0.0f;
        returnProgress = 0.0f;
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Caméra réinitialisée à la position/orientation de départ.");
    } else {
        Ogre::LogManager::getSingleton().logMessage("ERREUR: CameraFollower - La caméra n'est attachée à aucun SceneNode lors du reset.");
    }
}

