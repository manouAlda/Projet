#include "../Header/Player.hpp"
#include <iostream>
#include <bullet/btBulletDynamicsCommon.h> // Inclusion directe de Bullet

Player::Player(Ogre::SceneManager* sceneMgr, btDynamicsWorld* world, const Ogre::Vector3& position)
    : mSceneMgr(sceneMgr),
      mMoveSpeed(10.0f),
      mJumpForce(5.0f),
      mIsOnGround(false)
{
    // Création de l'entité visuelle
    mEntity = mSceneMgr->createEntity("player", "Mesh.mesh");
    mEntity->setCastShadows(true);
    
    // Création du nœud de scène
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
    mSceneNode->setPosition(position);
    mSceneNode->setScale(0.2f, 0.2f, 0.2f); // Mise à l'échelle pour correspondre à la taille voulue
    //mSceneNode->setScale(5,5,5);
    mSceneNode->attachObject(mEntity);
    
    // Animation par défaut
    mEntity->getAnimationState("idle")->setLoop(true);
    mEntity->getAnimationState("idle")->setEnabled(true);
    
    // Création de la forme de collision (capsule)
    btCollisionShape* shape = new btCapsuleShape(0.5f, 1.8f); // Rayon 0.5, hauteur 1.8
    
    // Création du motionState pour lier la physique à la visualisation
    btDefaultMotionState* motionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), 
                   btVector3(position.x, position.y, position.z))
    );
    
    // Calcul de l'inertie
    float mass = 80.0f; // 80 kg
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);
    
    // Création du corps rigide
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
    mBody = new btRigidBody(rbInfo);
    
    // Configurations spécifiques pour un personnage
    mBody->setAngularFactor(btVector3(0, 1, 0)); // Rotation uniquement autour de l'axe Y
    mBody->setFriction(0.5f);
    mBody->setRestitution(0.0f);
    
    // Ajout au monde physique
    world->addRigidBody(mBody);
    
    // Stocker un pointeur vers le nœud pour les mises à jour
    mBody->setUserPointer(mSceneNode);
}

Player::~Player()
{
    // Si mBody existe, le supprimer du monde de simulation et libérer la mémoire
    if (mBody) {
        // Récupérer et libérer le motion state
        btMotionState* motionState = mBody->getMotionState();
        if (motionState) {
            delete motionState;
        }
        
        // Récupérer et libérer la collision shape
        btCollisionShape* shape = mBody->getCollisionShape();
        if (shape) {
            delete shape;
        }
        
        // Libérer le corps rigide
        delete mBody;
        mBody = nullptr;
    }
    
    // Le SceneManager s'occupera de nettoyer l'entité et le nœud de scène
}

void Player::update(float deltaTime)
{
    // Vérifier si le joueur est au sol
    mIsOnGround = checkGroundContact();

    // Mise à jour des animations
    mEntity->getAnimationState("idle")->addTime(deltaTime);
    
    // Mise à jour de la position visuelle à partir de la position physique
    if (mBody && mSceneNode) {
        // Récupérer la transformation actuelle du corps rigide
        btTransform trans;
        mBody->getMotionState()->getWorldTransform(trans);
        
        // Appliquer la position au nœud de scène
        btVector3 pos = trans.getOrigin();
        mSceneNode->setPosition(pos.x(), pos.y(), pos.z());
        
        // Appliquer la rotation au nœud de scène
        btQuaternion rot = trans.getRotation();
        mSceneNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
    }
}

bool Player::checkGroundContact()
{
    // Implémenter une détection de collision avec le sol
    // On vérifie la vitesse verticale comme mesure approximative
    btVector3 velocity = mBody->getLinearVelocity();
    
    // On considère qu'on est au sol si la vitesse verticale est très faible
    return (std::abs(velocity.y()) < 0.1f);
    
    // Version plus avancée avec raycasting - nécessite une référence au monde
    // qui n'est pas disponible ici - il faudrait stocker une référence au monde
    /*
    btVector3 from = mBody->getWorldTransform().getOrigin();
    btVector3 to = from - btVector3(0, 0.6f, 0); // Vérifier 0.6 unités sous les pieds
    
    // Il faudrait avoir accès au monde de collision ici
    btCollisionWorld* collisionWorld = ...
    
    if (collisionWorld) {
        btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
        collisionWorld->rayTest(from, to, rayCallback);
        
        if (rayCallback.hasHit()) {
            return true;
        }
    }
    
    return false;
    */
}

void Player::moveLeft(float deltaTime)
{
    // Appliquer une force ou une impulsion vers la gauche
    btVector3 velocity = mBody->getLinearVelocity();
    btVector3 impulse(-mMoveSpeed, 0, 0);
    
    // Conserver la vitesse verticale actuelle
    impulse = btVector3(impulse.x(), velocity.y(), impulse.z());
    
    // Définir directement la vélocité au lieu d'appliquer une impulsion
    // Cela donne un contrôle plus direct
    mBody->setLinearVelocity(impulse);
    
    // Orienter le personnage vers la gauche
    mSceneNode->setDirection(-1, 0, 0, Ogre::Node::TS_WORLD);
}

void Player::moveRight(float deltaTime)
{
    // Appliquer une force ou une impulsion vers la droite
    btVector3 velocity = mBody->getLinearVelocity();
    btVector3 impulse(mMoveSpeed, 0, 0);
    
    // Conserver la vitesse verticale actuelle
    impulse = btVector3(impulse.x(), velocity.y(), impulse.z());
    
    // Définir directement la vélocité
    mBody->setLinearVelocity(impulse);
    
    // Orienter le personnage vers la droite
    mSceneNode->setDirection(1, 0, 0, Ogre::Node::TS_WORLD);
}

void Player::moveForward(float deltaTime)
{
    // Appliquer une force ou une impulsion vers l'avant
    btVector3 velocity = mBody->getLinearVelocity();
    btVector3 impulse(0, 0, -mMoveSpeed);
    
    // Conserver la vitesse verticale actuelle
    impulse = btVector3(impulse.x(), velocity.y(), impulse.z());
    
    // Définir directement la vélocité
    mBody->setLinearVelocity(impulse);
    
    // Orienter le personnage vers l'avant
    mSceneNode->setDirection(0, 0, -1, Ogre::Node::TS_WORLD);
}

void Player::moveBackward(float deltaTime)
{
    // Appliquer une force ou une impulsion vers l'arrière
    btVector3 velocity = mBody->getLinearVelocity();
    btVector3 impulse(0, 0, mMoveSpeed);
    
    // Conserver la vitesse verticale actuelle
    impulse = btVector3(impulse.x(), velocity.y(), impulse.z());
    
    // Définir directement la vélocité
    mBody->setLinearVelocity(impulse);
    
    // Orienter le personnage vers l'arrière
    mSceneNode->setDirection(0, 0, 1, Ogre::Node::TS_WORLD);
}

void Player::jump()
{
    // Vérifier si le joueur est au sol avant de sauter
    if (mIsOnGround) {
        // Appliquer une impulsion vers le haut
        btVector3 velocity = mBody->getLinearVelocity();
        btVector3 jumpImpulse(velocity.x(), mJumpForce, velocity.z());
        mBody->setLinearVelocity(jumpImpulse);
        
        // Marquer comme n'étant plus au sol
        mIsOnGround = false;
    }
}

Ogre::Vector3 Player::getPosition() const {
    if (mSceneNode) {
        return mSceneNode->getPosition();
    }
    return Ogre::Vector3::ZERO;
}

bool Player::isOnGround() const {
    return mIsOnGround;
}

Ogre::SceneNode* Player::getSceneNode() const {
    return mSceneNode;
}

btRigidBody* Player::getRigidBody() const {
    return mBody;
}
    