#include "../../include/managers/CameraFollower.h"
#include <OgreMath.h>

CameraFollower::CameraFollower(Ogre::Camera* camera, BowlingBall* ball)
    : camera(camera),
      cameraNode(nullptr),
      ball(ball),
      following(false),
      initialPosition(0.0f, 3.0f, -16.5f), 
      initialOrientation(Ogre::Quaternion::IDENTITY), 
      pinsLookAt(0.0f, 0.5f, -10.0f), 
      targetPosition(Ogre::Vector3::ZERO),
      lookAtTarget(Ogre::Vector3::ZERO),
      transitionSpeed(4.0f), // Vitesse de suivi
      offset(0.0f, 1.5f, 3.0f), // Décalage caméra/boule pendant le suivi
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
            cameraNode->lookAt(Ogre::Vector3(0, 0.5, 0), Ogre::Node::TS_WORLD);
            initialOrientation = cameraNode->getOrientation();
            Ogre::LogManager::getSingleton().logMessage("CameraFollower: Orientation initiale définie.");
        }
    }
}

CameraFollower::~CameraFollower() {}

void CameraFollower::initialize() {
    resetToStartPosition(true); 
    Ogre::LogManager::getSingleton().logMessage("CameraFollower initialisé.");
}

void CameraFollower::update(float deltaTime) {
    if (!camera || !cameraNode || !ball) {
        return;
    }

    if (ball->isRolling() == false && postRollState == PostRollState::NONE) {
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Boule arrêtée. Début séquence post-lancer.");
        following = false;
        
        // Calculer une position derrière la boule pour la caméra
        Ogre::Vector3 ballPosition = ball->getPosition();
        
        // Déterminer la direction depuis les quilles vers la boule (direction opposée à la piste)
        Ogre::Vector3 directionFromPins = (ballPosition - pinsLookAt).normalisedCopy();
        
        // Positionner la caméra derrière la boule (dans la direction opposée à la piste)
        Ogre::Vector3 cameraPosition = ballPosition + directionFromPins * 5.0f; // 5 unités derrière la boule
        cameraPosition.y = ballPosition.y + 2.0f; // Un peu au-dessus de la boule
        
        // Déplacer la caméra à cette nouvelle position
        cameraNode->setPosition(cameraPosition);
        
        // Orienter la caméra pour regarder la boule
        Ogre::Vector3 direction = ballPosition - cameraPosition;
        Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y;
        
        Ogre::Vector3 zAxis = -direction.normalisedCopy();
        Ogre::Vector3 xAxis = upVector.crossProduct(zAxis).normalisedCopy();
        Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis).normalisedCopy();
        
        Ogre::Quaternion orientation;
        orientation.FromAxes(xAxis, yAxis, zAxis);
        cameraNode->setOrientation(orientation);
        
        postRollState = PostRollState::FOCUS_BALL;
        postRollTimer = 0.0f;
        lookAtTarget = ballPosition;
        
        // Ajouter un log pour confirmer que cette section est exécutée
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Caméra repositionnée derrière la boule.");
    }


    if (following) {
        // Le code existant pour le suivi de la boule...
        Ogre::Vector3 ballPosition = ball->getPosition();
        // Position cible légèrement derrière et au-dessus de la boule
        targetPosition = ballPosition + cameraNode->getOrientation() * offset;
        targetPosition.y = std::max(targetPosition.y, 0.5f);

        lookAtTarget = ballPosition + ball->getVelocity() * 0.1f; 

        // Interpolation fluide vers la position cible
        Ogre::Vector3 currentPos = cameraNode->getPosition();
        Ogre::Vector3 newPos = currentPos + (targetPosition - currentPos) * (transitionSpeed * deltaTime);
        cameraNode->setPosition(newPos);
        cameraNode->lookAt(lookAtTarget, Ogre::Node::TS_WORLD);

        // Nous avons déplacé cette vérification au début de la fonction
        // if (!ball->isRolling()) { ... }
    }
    else if (postRollState != PostRollState::NONE) {
        postRollTimer += deltaTime;

        switch (postRollState) {
            case PostRollState::FOCUS_BALL:
                cameraNode->lookAt(lookAtTarget, Ogre::Node::TS_WORLD);
                if (postRollTimer >= FOCUS_BALL_DURATION) {
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Fin focus boule, début focus quilles.");
                    postRollState = PostRollState::FOCUS_PINS;
                    postRollTimer = 0.0f; 
                }
                break;

            case PostRollState::FOCUS_PINS:
            {
        // Calculer une orientation pour regarder vers les quilles depuis la position actuelle
        Ogre::Vector3 direction = pinsLookAt - cameraNode->getPosition();
        Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y;
        
        Ogre::Vector3 zAxis = -direction.normalisedCopy();
        Ogre::Vector3 xAxis = upVector.crossProduct(zAxis).normalisedCopy();
        Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis).normalisedCopy();
        
        Ogre::Quaternion targetOrientation;
        targetOrientation.FromAxes(xAxis, yAxis, zAxis);
        
        // Slerp pour une rotation fluide, mais sans changer la position
        Ogre::Quaternion currentOrientation = cameraNode->getOrientation();
        Ogre::Quaternion newOrientation = Ogre::Quaternion::Slerp(
            postRollTimer / FOCUS_PINS_DURATION,
            currentOrientation, targetOrientation, true);
        cameraNode->setOrientation(newOrientation);
    }
    
    if (postRollTimer >= FOCUS_PINS_DURATION) {
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Fin focus quilles, début retour.");
        postRollState = PostRollState::RETURNING;
        postRollTimer = 0.0f;
        returnProgress = 0.0f;
        returnStartPosition = cameraNode->getPosition();
        returnStartOrientation = cameraNode->getOrientation();
    }
            break;


            case PostRollState::RETURNING:
                returnProgress += deltaTime * RETURN_SPEED;
                returnProgress = std::min(returnProgress, 1.0f); // Clamp à 1.0

                // Interpolation linéaire pour la position
                cameraNode->setPosition(returnStartPosition + (initialPosition - returnStartPosition) * returnProgress);
                // Interpolation sphérique (Slerp) pour l'orientation
                cameraNode->setOrientation(Ogre::Quaternion::Slerp(returnProgress, returnStartOrientation, initialOrientation, true));

                if (returnProgress >= 1.0f) {
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Retour à la position initiale terminé.");
                    postRollState = PostRollState::NONE;
                    cameraNode->setPosition(initialPosition);
                    cameraNode->setOrientation(initialOrientation);
                }
                break;
            
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

void CameraFollower::resetToStartPosition(bool initialSetup) {
    static bool& ballStopHandled = *new bool(false);
    ballStopHandled = false;
    
    if (cameraNode) {
        cameraNode->setPosition(initialPosition);
        
        if (initialSetup) {
            // Pour l'initialisation au démarrage du jeu :
            // Utiliser la méthode FromAxes pour orienter correctement la caméra
            cameraNode->lookAt(pinsLookAt, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Y);
            initialOrientation = cameraNode->getOrientation();
            
            // Ajouter une séquence d'introduction qui montre la piste puis la boule
            postRollState = PostRollState::FOCUS_PINS;
            postRollTimer = 0.0f;
            Ogre::LogManager::getSingleton().logMessage("CameraFollower: Initialisation avec séquence d'introduction.");
        } else {
            // Pour les réinitialisations pendant le jeu :
            // Simplement revenir à l'orientation initiale sans séquence
            cameraNode->setOrientation(initialOrientation);
            postRollState = PostRollState::NONE;
            postRollTimer = 0.0f;
            returnProgress = 0.0f;
            Ogre::LogManager::getSingleton().logMessage("CameraFollower: Caméra réinitialisée à la position/orientation de départ.");
        }
        
        following = false;
    } else {
        Ogre::LogManager::getSingleton().logMessage("ERREUR: CameraFollower - La caméra n'est attachée à aucun SceneNode lors du reset.");
    }
}
*********************************************************************************************************************************************************************************************************
Version 1 
*****************************************************************************************************************************************************************
#include "../../include/managers/CameraFollower.h"
#include <OgreMath.h>

CameraFollower::CameraFollower(Ogre::Camera* camera, BowlingBall* ball)
    : camera(camera),
      cameraNode(nullptr),
      ball(ball),
      following(false),
      initialPosition(0.0f, 3.0f, -16.5f), 
      initialOrientation(Ogre::Quaternion::IDENTITY), 
      pinsLookAt(0.0f, 0.5f, -10.0f), 
      targetPosition(Ogre::Vector3::ZERO),
      lookAtTarget(Ogre::Vector3::ZERO),
      transitionSpeed(4.0f), // Vitesse de suivi
      offset(0.0f, 1.5f, 3.0f), // Décalage caméra/boule pendant le suivi
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
            cameraNode->lookAt(Ogre::Vector3(0, 0.5, 0), Ogre::Node::TS_WORLD);
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

    if (ball->isRolling() == false && postRollState == PostRollState::NONE) {
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Boule arrêtée. Début séquence post-lancer.");
        following = false;
        
        // Calculer une position derrière la boule pour la caméra
        Ogre::Vector3 ballPosition = ball->getPosition();
        
        // Déterminer la direction depuis les quilles vers la boule (direction opposée à la piste)
        Ogre::Vector3 directionFromPins = (ballPosition - pinsLookAt).normalisedCopy();
        
        // Positionner la caméra derrière la boule (dans la direction opposée à la piste)
        Ogre::Vector3 cameraPosition = ballPosition + directionFromPins * 5.0f; // 5 unités derrière la boule
        cameraPosition.y = ballPosition.y + 2.0f; // Un peu au-dessus de la boule
        
        // Déplacer la caméra à cette nouvelle position
        cameraNode->setPosition(cameraPosition);
        
        // Orienter la caméra pour regarder la boule
        Ogre::Vector3 direction = ballPosition - cameraPosition;
        Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y;
        
        Ogre::Vector3 zAxis = -direction.normalisedCopy();
        Ogre::Vector3 xAxis = upVector.crossProduct(zAxis).normalisedCopy();
        Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis).normalisedCopy();
        
        Ogre::Quaternion orientation;
        orientation.FromAxes(xAxis, yAxis, zAxis);
        cameraNode->setOrientation(orientation);
        
        postRollState = PostRollState::FOCUS_BALL;
        postRollTimer = 0.0f;
        lookAtTarget = ballPosition;
        
        // Ajouter un log pour confirmer que cette section est exécutée
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Caméra repositionnée derrière la boule.");
    }


    if (following) {
        // Le code existant pour le suivi de la boule...
        Ogre::Vector3 ballPosition = ball->getPosition();
        // Position cible légèrement derrière et au-dessus de la boule
        targetPosition = ballPosition + cameraNode->getOrientation() * offset;
        targetPosition.y = std::max(targetPosition.y, 0.5f);

        lookAtTarget = ballPosition + ball->getVelocity() * 0.1f; 

        // Interpolation fluide vers la position cible
        Ogre::Vector3 currentPos = cameraNode->getPosition();
        Ogre::Vector3 newPos = currentPos + (targetPosition - currentPos) * (transitionSpeed * deltaTime);
        cameraNode->setPosition(newPos);
        cameraNode->lookAt(lookAtTarget, Ogre::Node::TS_WORLD);

        // Nous avons déplacé cette vérification au début de la fonction
        // if (!ball->isRolling()) { ... }
    }
    else if (postRollState != PostRollState::NONE) {
        postRollTimer += deltaTime;

        switch (postRollState) {
            case PostRollState::FOCUS_BALL:
                cameraNode->lookAt(lookAtTarget, Ogre::Node::TS_WORLD);
                if (postRollTimer >= FOCUS_BALL_DURATION) {
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Fin focus boule, début focus quilles.");
                    postRollState = PostRollState::FOCUS_PINS;
                    postRollTimer = 0.0f; 
                }
                break;

            case PostRollState::FOCUS_PINS:
            {
        // Calculer une orientation pour regarder vers les quilles depuis la position actuelle
        Ogre::Vector3 direction = pinsLookAt - cameraNode->getPosition();
        Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y;
        
        Ogre::Vector3 zAxis = -direction.normalisedCopy();
        Ogre::Vector3 xAxis = upVector.crossProduct(zAxis).normalisedCopy();
        Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis).normalisedCopy();
        
        Ogre::Quaternion targetOrientation;
        targetOrientation.FromAxes(xAxis, yAxis, zAxis);
        
        // Slerp pour une rotation fluide, mais sans changer la position
        Ogre::Quaternion currentOrientation = cameraNode->getOrientation();
        Ogre::Quaternion newOrientation = Ogre::Quaternion::Slerp(
            postRollTimer / FOCUS_PINS_DURATION,
            currentOrientation, targetOrientation, true);
        cameraNode->setOrientation(newOrientation);
    }
    
    if (postRollTimer >= FOCUS_PINS_DURATION) {
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Fin focus quilles, début retour.");
        postRollState = PostRollState::RETURNING;
        postRollTimer = 0.0f;
        returnProgress = 0.0f;
        returnStartPosition = cameraNode->getPosition();
        returnStartOrientation = cameraNode->getOrientation();
    }
            break;


            case PostRollState::RETURNING:
                returnProgress += deltaTime * RETURN_SPEED;
                returnProgress = std::min(returnProgress, 1.0f); // Clamp à 1.0

                // Interpolation linéaire pour la position
                cameraNode->setPosition(returnStartPosition + (initialPosition - returnStartPosition) * returnProgress);
                // Interpolation sphérique (Slerp) pour l'orientation
                cameraNode->setOrientation(Ogre::Quaternion::Slerp(returnProgress, returnStartOrientation, initialOrientation, true));

                if (returnProgress >= 1.0f) {
                    Ogre::LogManager::getSingleton().logMessage("CameraFollower: Retour à la position initiale terminé.");
                    postRollState = PostRollState::NONE;
                    cameraNode->setPosition(initialPosition);
                    cameraNode->setOrientation(initialOrientation);
                }
                break;
            
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
        // >>> Début de la modification <<<
        // Utiliser les positions définies dans le constructeur/membres
        cameraNode->setPosition(initialPosition); // initialPosition = (0.0f, 3.0f, -16.5f)

        Ogre::Vector3 direction = pinsLookAt - initialPosition;
        Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y; // Assumer Y comme "haut"

        //Ogre::Vector3 zAxis = direction.normalisedCopy();
        
        Ogre::Vector3 zAxis = -direction.normalisedCopy(); // Inverser la direction pour l'axe Z
        
        Ogre::Vector3 xAxis = upVector.crossProduct(zAxis).normalisedCopy();
        Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis).normalisedCopy();

        Ogre::Quaternion orientation;
        orientation.FromAxes(xAxis, yAxis, zAxis);
        cameraNode->setOrientation(orientation);

        // Sauvegarder cette orientation comme l'orientation initiale pour le retour
        initialOrientation = orientation;

        following = false;
        postRollState = PostRollState::NONE;
        postRollTimer = 0.0f;
        returnProgress = 0.0f;
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Caméra réinitialisée à la position/orientation de départ.");
    } else {
        Ogre::LogManager::getSingleton().logMessage("ERREUR: CameraFollower - La caméra n'est attachée à aucun SceneNode lors du reset.");
    }
}

void CameraFollower::setCameraPosition(const Ogre::Vector3& position) {
    if (cameraNode) {
        cameraNode->setPosition(position);
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Position forcée à " + 
            Ogre::StringConverter::toString(position));
    }
}

void CameraFollower::setCameraLookAt(const Ogre::Vector3& target) {
    if (cameraNode) {
        // Calculer l'orientation pour regarder la cible
        Ogre::Vector3 direction = target - cameraNode->getPosition();
        Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y;
        
        Ogre::Vector3 zAxis = -direction.normalisedCopy();
        Ogre::Vector3 xAxis = upVector.crossProduct(zAxis).normalisedCopy();
        Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis).normalisedCopy();
        
        Ogre::Quaternion orientation;
        orientation.FromAxes(xAxis, yAxis, zAxis);
        cameraNode->setOrientation(orientation);
        
        Ogre::LogManager::getSingleton().logMessage("CameraFollower: Orientation forcée pour regarder " + 
            Ogre::StringConverter::toString(target));
    }
}
