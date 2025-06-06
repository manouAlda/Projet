#include "../../include/objects/BowlingPin.h"

BowlingPin::BowlingPin(Ogre::SceneManager* sceneMgr)
    : sceneMgr(sceneMgr), 
      pinNode(nullptr), 
      pinEntity(nullptr), 
      pinBody(nullptr), 
      initialPosition(Ogre::Vector3::ZERO) {}

BowlingPin::~BowlingPin() {
    if (pinBody) {
        PhysicsManager::getInstance()->getDynamicsWorld()->getBtWorld()->removeRigidBody(pinBody);
    }
}

void BowlingPin::create(const Ogre::Vector3& position, int pinIndex) {
    // Sauvegarde de la position initiale
    initialPosition = position;
    
    std::string nodeName = "BowlingPinNode_" + std::to_string(pinIndex);
    std::string entityName = "BowlingPinEntity_" + std::to_string(pinIndex);

    pinNode = sceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
    pinEntity = sceneMgr->createEntity(entityName, "pin.mesh"); 
    pinNode->attachObject(pinEntity);
    pinNode->setPosition(position);
    
    float scale = 0.1f;  
    //pinNode->setScale(scale, scale, scale);
    
    pinBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        1.5f, pinEntity, Ogre::Bullet::CT_HULL);  
    
    // Propriétés physiques essentielles
    if (pinBody) {
        btVector3 inertia;
        btCollisionShape* shape = pinBody->getCollisionShape();
        shape->calculateLocalInertia(1.5f, inertia);
        pinBody->setMassProps(1.5f, inertia);  // Masse réaliste
        
        pinBody->setFriction(0.6f);             // Friction pour ne pas glisser
        pinBody->setRollingFriction(0.1f);      // Friction de roulement
        pinBody->setSpinningFriction(0.1f);     // Friction de rotation
        pinBody->setRestitution(0.3f);          // Rebond modéré
        pinBody->setActivationState(ACTIVE_TAG); // Activation physique
        
        // Désactiver la désactivation automatique pour éviter que les quilles ne "s'endorment"
        pinBody->setDeactivationTime(30.0f);     // Temps plus long avant désactivation
    }
}

void BowlingPin::reset() {
    if (pinNode && pinBody) {
        // Réinitialiser à la position d'origine sauvegardée
        pinNode->setPosition(initialPosition);
        
        // Réinitialiser la vitesse et la rotation
        pinBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        pinBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        
        // Réinitialiser la rotation du nœud
        pinNode->setOrientation(Ogre::Quaternion::IDENTITY);
        
        // S'assurer que le corps physique est réactivé
        pinBody->activate(true);
    }
}

void BowlingPin::update(float deltaTime) {
    if (pinBody && pinNode) {
        // Mise à jour de la position du nœud Ogre à partir de la position du corps rigide Bullet
        btTransform transform;
        pinBody->getMotionState()->getWorldTransform(transform);
        
        btVector3 position = transform.getOrigin();
        pinNode->setPosition(position.x(), position.y(), position.z());
        
        // Mise à jour de la rotation du nœud Ogre à partir de la rotation du corps rigide Bullet
        btQuaternion rotation = transform.getRotation();
        pinNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    }
}

bool BowlingPin::isKnockedDown() const {
    if (pinNode) {
        // Une quille est considérée comme renversée si elle est inclinée de plus de 45 degrés
        Ogre::Vector3 upVector = pinNode->getOrientation() * Ogre::Vector3::UNIT_Y;
        float dotProduct = upVector.dotProduct(Ogre::Vector3::UNIT_Y);
        Ogre::LogManager::getSingleton().logMessage("Pin " + pinNode->getName() + " dotProduct: " + Ogre::StringConverter::toString(dotProduct));
        return dotProduct < 0.7071f;  // cos(45°) ≈ 0.7071
    }
    return false;
}

btRigidBody* BowlingPin::getPinBody() const {
    return pinBody;
}

Ogre::SceneNode* BowlingPin::getPinNode() const {
    return pinNode;
}