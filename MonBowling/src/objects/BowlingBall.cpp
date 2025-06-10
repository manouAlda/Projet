#include "../../include/objects/BowlingBall.h"
#include "../../include/managers/PhysicsManager.h" 
#include <OgreLogManager.h>
#include <OgreStringConverter.h>

BowlingBall::BowlingBall(Ogre::SceneManager* sceneMgr, const Ogre::String& meshName)
    : sceneMgr(sceneMgr),
      ballNode(nullptr),
      ballEntity(nullptr),
      ballBody(nullptr),
      radius(0.108f), 
      mass(7.0f),     
      rolling(false),
      initialPosition(Ogre::Vector3::ZERO),
      scale(0.02f) 
{}

BowlingBall::~BowlingBall() {
    if (ballBody) {
        auto* world = PhysicsManager::getInstance()->getDynamicsWorld()->getBtWorld();
        if (world) {
            world->removeRigidBody(ballBody);
        }
        ballBody = nullptr; 
    }
}

void BowlingBall::create(const Ogre::Vector3& position) {
    auto physicsManager = PhysicsManager::getInstance();
    if (!physicsManager || !physicsManager->getDynamicsWorld()) {
        Ogre::LogManager::getSingleton().logError("BowlingBall::create - PhysicsManager ou DynamicsWorld non initialisé");
        return;
    }

    initialPosition = position;

    // Création du nœud de scène
    ballNode = sceneMgr->getRootSceneNode()->createChildSceneNode("BowlingBallNode");
    ballNode->setPosition(position);


    // Création de l'entité
    try {
        ballEntity = sceneMgr->createEntity("BowlingBallEntity", "BowlingBall.mesh");
        ballNode->attachObject(ballEntity);
        ballNode->setScale(scale, scale, scale);
    } catch (Ogre::Exception& e) {
        Ogre::LogManager::getSingleton().logError("BowlingBall::create - Erreur création entité: " + Ogre::String(e.getFullDescription().c_str()));
        sceneMgr->destroySceneNode(ballNode);
        ballNode = nullptr;
        return;
    }

    // Création du corps rigide
    try {
        ballBody = physicsManager->getDynamicsWorld()->addRigidBody(
                mass, ballEntity, Ogre::Bullet::CT_SPHERE);

        if (ballBody) {
            ballBody->setFriction(0.3f);
            ballBody->setRestitution(0.3f);
            ballBody->setRollingFriction(0.3f);
            ballBody->setSpinningFriction(0.2f);

            // Empêcher la désactivation pour que la boule continue de rouler
            ballBody->setActivationState(DISABLE_DEACTIVATION);
            Ogre::LogManager::getSingleton().logMessage("BowlingBall::create - Corps rigide créé avec succès.");
        } else {
             Ogre::LogManager::getSingleton().logError("BowlingBall::create - addRigidBody a retourné nullptr.");
        }
    } catch (Ogre::Exception& e) {
        Ogre::LogManager::getSingleton().logError("BowlingBall::create - Erreur création corps rigide: " + Ogre::String(e.getFullDescription().c_str()));
        ballNode->detachObject(ballEntity);
        sceneMgr->destroyEntity(ballEntity);
        sceneMgr->destroySceneNode(ballNode);
        ballEntity = nullptr;
        ballNode = nullptr;
        return;
    }
}

void BowlingBall::reset() {
    if (ballNode && ballBody) {
        // Réinitialiser la position et l'orientation Ogre
        ballNode->setPosition(initialPosition);
        ballNode->setOrientation(Ogre::Quaternion::IDENTITY);
        Ogre::LogManager::getSingleton().logMessage("[BowlingBall::reset] Noeud Ogre réinitialisé à : " +
            Ogre::StringConverter::toString(initialPosition));

        ballBody->setLinearVelocity(btVector3(0, 0, 0));
        ballBody->setAngularVelocity(btVector3(0, 0, 0));
        ballBody->clearForces(); // Important pour annuler toute force résiduelle

        // Réinitialiser la transformation du monde physique
        btTransform initialTransform;
        initialTransform.setIdentity();
        initialTransform.setOrigin(btVector3(initialPosition.x, initialPosition.y, initialPosition.z));

        // Appliquer la transformation au corps et à son état de mouvement
        ballBody->setWorldTransform(initialTransform);
        if (ballBody->getMotionState()) {
            ballBody->getMotionState()->setWorldTransform(initialTransform);
        }

        // Réactiver le corps si nécessaire (même si on utilise DISABLE_DEACTIVATION)
        ballBody->activate(true);

        Ogre::LogManager::getSingleton().logMessage("[BowlingBall::reset] Corps physique réinitialisé à : " +
            Ogre::StringConverter::toString(initialPosition));

        rolling = false;
    } else {
        Ogre::LogManager::getSingleton().logWarning("[BowlingBall::reset] Tentative de réinitialisation mais ballNode ou ballBody est nul.");
    }
}

void BowlingBall::launch(const Ogre::Vector3& direction, float power, float spin) {
    if (ballBody) {
        Ogre::Vector3 normalizedDir = direction.normalisedCopy();
        
        // Application de la force linéaire (vitesse initiale)
        btVector3 velocity(normalizedDir.x * power,
                          normalizedDir.y * power, 
                          normalizedDir.z * power);
        ballBody->setLinearVelocity(velocity);
        
        // SPIN SIMPLIFIÉ - Rotation uniquement autour de l'axe Y (vertical)
        // C'est le type de spin principal au bowling
        btVector3 angularVelocity(
            0.0f,      // Pas de rotation sur X
            spin*20.0f,      // Rotation principale sur Y (effet hook/curve)
            0.0f       // Pas de rotation sur Z
        );
        
        ballBody->setAngularVelocity(angularVelocity);
        
        // CRUCIAL: Vérifier et ajuster les propriétés de friction
        ballBody->setFriction(0.8f);        // Friction élevée
        ballBody->setRollingFriction(0.3f);  // Friction de roulement
        ballBody->setSpinningFriction(0.2f); // Friction de spin
        
        // Réactiver le corps
        ballBody->activate(true);
        rolling = true;
        
        Ogre::LogManager::getSingleton().logMessage(
            "BowlingBall::launch - Vitesse: " + Ogre::StringConverter::toString(power) +
            " Spin Y: " + Ogre::StringConverter::toString(spin) +
            " Direction: " + Ogre::StringConverter::toString(normalizedDir));
    }
}

void BowlingBall::update(float deltaTime) {
    if (ballBody && ballNode && rolling) {
        // --- Synchronisation Ogre <-> Bullet --- 
        btTransform transform;
        // Utiliser getWorldTransform directement peut être plus fiable que getMotionState
        transform = ballBody->getWorldTransform();
        // if (ballBody->getMotionState()) {
        //     ballBody->getMotionState()->getWorldTransform(transform);
        // } else {
        //     transform = ballBody->getWorldTransform();
        // }

        // Mise à jour de la position Ogre
        btVector3 position = transform.getOrigin();
        ballNode->setPosition(position.x(), position.y(), position.z());

        // Mise à jour de la rotation Ogre
        btQuaternion rotation = transform.getRotation();
        ballNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());

        updateSpin(deltaTime);

        btVector3 velocity = ballBody->getLinearVelocity();
        btVector3 angularVelocity = ballBody->getAngularVelocity();
        float linearSpeedSq = velocity.length2();
        float angularSpeedSq = angularVelocity.length2();

        // Ajouter ce log toutes les 30 frames environ pour ne pas surcharger la console
        static int frameCount = 0;
        if (++frameCount % 10 == 0) {
            Ogre::LogManager::getSingleton().logMessage("BowlingBall::update - Vitesse linéaire: " + 
                Ogre::StringConverter::toString(sqrt(linearSpeedSq)) + 
                ", Vitesse angulaire: " + Ogre::StringConverter::toString(sqrt(angularSpeedSq)) +
                ", Seuil: " + Ogre::StringConverter::toString(STOP_VELOCITY_THRESHOLD));
        }

        // --- Vérification des conditions d'arrêt --- 

        // 1. Vérification de la limite z
        if (position.z() <= -15.0f) {
            Ogre::LogManager::getSingleton().logMessage("BowlingBall::update - Limite Z atteinte (" +
                Ogre::StringConverter::toString(position.y()) + "). Arrêt de la boule.");
            ballBody->setLinearVelocity(btVector3(0, 0, 0));
            ballBody->setAngularVelocity(btVector3(0, 0, 0));
            rolling = false;
        }
        // 2. Vérification de l'arrêt par faible vélocité (seulement si pas déjà arrêté par Y)
        else {
            btVector3 velocity = ballBody->getLinearVelocity();
            btVector3 angularVelocity = ballBody->getAngularVelocity();

            float linearSpeedSq = velocity.length2(); // Utiliser length2() est plus rapide
            float angularSpeedSq = angularVelocity.length2();

            // Comparer avec le carré du seuil
            if (linearSpeedSq < (STOP_VELOCITY_THRESHOLD * STOP_VELOCITY_THRESHOLD) &&
                angularSpeedSq < (STOP_VELOCITY_THRESHOLD * STOP_VELOCITY_THRESHOLD)) {
                Ogre::LogManager::getSingleton().logMessage("BowlingBall::update - Faible vélocité détectée. Arrêt de la boule.");
                ballBody->setLinearVelocity(btVector3(0, 0, 0));
                ballBody->setAngularVelocity(btVector3(0, 0, 0));
                rolling = false;
            }
        }

    }
}

void BowlingBall::updateSpin(float deltaTime) {
    if (ballBody && rolling) {
        // Récupérer la vélocité actuelle
        btVector3 currentVelocity = ballBody->getLinearVelocity();
        btVector3 currentAngular = ballBody->getAngularVelocity();
        
        // Calculer la vitesse de la boule
        float speed = currentVelocity.length();
        
        // Si la boule roule encore (vitesse > seuil minimal)
        if (speed > 0.1f) {
            // Appliquer une friction différentielle basée sur le spin
            // Cela permet à la boule de "hooker" (courber sa trajectoire)
            
            // Calculer la direction actuelle
            btVector3 direction = currentVelocity.normalized();
            
            // Force latérale basée sur le spin Y (effet Magnus simplifié)
            float spinEffect = currentAngular.y() * 0.3f; // Facteur d'effet
            
            // Vecteur perpendiculaire à la direction (pour la force latérale)
            btVector3 rightVector(-direction.z(), 0, direction.x());
            
            // Appliquer une force latérale
            btVector3 sideForce = rightVector * spinEffect * speed * deltaTime;
            ballBody->applyCentralForce(sideForce);
            
            // Réduire progressivement le spin (friction)
            btVector3 dampedAngular = currentAngular * (1.0f - deltaTime * 0.005f);
            ballBody->setAngularVelocity(dampedAngular);
        }
    }
}

// --- Getters --- 

float BowlingBall::getRadius() const {
    return radius; // Assurez-vous que `radius` est une variable membre de la classe
}

Ogre::Vector3 BowlingBall::getPosition() const {
    if (ballNode) {
        return ballNode->getPosition();
    }
    return Ogre::Vector3::ZERO;
}

Ogre::Vector3 BowlingBall::getVelocity() const {
    if (ballBody) {
        btVector3 velocity = ballBody->getLinearVelocity();
        return Ogre::Vector3(velocity.x(), velocity.y(), velocity.z());
    }
    return Ogre::Vector3::ZERO;
}

SpinInfo BowlingBall::getCurrentSpinInfo() const {
    SpinInfo info;
    if (ballBody) {
        btVector3 angular = ballBody->getAngularVelocity();
        info.spinY = angular.y();
        info.spinX = angular.x();
        info.spinZ = angular.z();
        info.totalSpin = angular.length();
    }
    return info;
}

// --- Setters --- 

void BowlingBall::setMass(float mass) {
    mass = mass;
    if (ballBody) {
        btVector3 inertia(0,0,0);
        // Recalculer l'inertie si la forme existe
        if(ballBody->getCollisionShape()){
            ballBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
        }
        ballBody->setMassProps(mass, inertia);
    }
}

void BowlingBall::setFriction(float friction) {
    if (ballBody) {
        ballBody->setFriction(friction);
    }
}

void BowlingBall::setRestitution(float restitution) {
    if (ballBody) {
        ballBody->setRestitution(restitution);
    }
}

void BowlingBall::setRollingFriction(float rollingFriction) {
    if (ballBody) {
        ballBody->setRollingFriction(rollingFriction);
    }
}

void BowlingBall::setSpinningFriction(float spinningFriction) {
    if (ballBody) {
        ballBody->setSpinningFriction(spinningFriction);
    }
}

