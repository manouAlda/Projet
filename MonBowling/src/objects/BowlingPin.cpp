#include "../../include/objects/BowlingPin.h"

BowlingPin::BowlingPin(Ogre::SceneManager* sceneMgr)
    : mSceneMgr(sceneMgr), 
      mPinNode(nullptr), 
      mPinEntity(nullptr), 
      mPinBody(nullptr), 
      mInitialPosition(Ogre::Vector3::ZERO) {}

BowlingPin::~BowlingPin() {
    if (mPinBody) {
        PhysicsManager::getInstance()->getDynamicsWorld()->getBtWorld()->removeRigidBody(mPinBody);
    }
}

void BowlingPin::create(const Ogre::Vector3& position, int pinIndex) {
    // Sauvegarde de la position initiale
    mInitialPosition = position;
    
    std::string nodeName = "BowlingPinNode_" + std::to_string(pinIndex);
    std::string entityName = "BowlingPinEntity_" + std::to_string(pinIndex);

    mPinNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
    mPinEntity = mSceneMgr->createEntity(entityName, "pin.mesh"); 
    mPinNode->attachObject(mPinEntity);
    mPinNode->setPosition(position);
    
    float scale = 1.0f;  
    mPinNode->setScale(scale, scale, scale);
    
    mPinBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        1.5f, mPinEntity, Ogre::Bullet::CT_HULL);  
    
    // Propriétés physiques essentielles
    if (mPinBody) {
        btVector3 inertia;
        btCollisionShape* shape = mPinBody->getCollisionShape();
        shape->calculateLocalInertia(1.5f, inertia);
        mPinBody->setMassProps(1.5f, inertia);  // Masse réaliste
        
        mPinBody->setFriction(0.6f);             // Friction pour ne pas glisser
        mPinBody->setRollingFriction(0.1f);      // Friction de roulement
        mPinBody->setSpinningFriction(0.1f);     // Friction de rotation
        mPinBody->setRestitution(0.3f);          // Rebond modéré
        mPinBody->setActivationState(ACTIVE_TAG); // Activation physique
        
        // Désactiver la désactivation automatique pour éviter que les quilles ne "s'endorment"
        mPinBody->setDeactivationTime(30.0f);     // Temps plus long avant désactivation
    }
}

void BowlingPin::reset() {
    if (mPinNode && mPinBody) {
        // Réinitialiser à la position d'origine sauvegardée
        mPinNode->setPosition(mInitialPosition);
        
        // Réinitialiser la vitesse et la rotation
        mPinBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        mPinBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        
        // Réinitialiser la rotation du nœud
        mPinNode->setOrientation(Ogre::Quaternion::IDENTITY);
        
        // S'assurer que le corps physique est réactivé
        mPinBody->activate(true);
    }
}

void BowlingPin::update(float deltaTime) {
    if (mPinBody && mPinNode) {
        // Mise à jour de la position du nœud Ogre à partir de la position du corps rigide Bullet
        btTransform transform;
        mPinBody->getMotionState()->getWorldTransform(transform);
        
        btVector3 position = transform.getOrigin();
        mPinNode->setPosition(position.x(), position.y(), position.z());
        
        // Mise à jour de la rotation du nœud Ogre à partir de la rotation du corps rigide Bullet
        btQuaternion rotation = transform.getRotation();
        mPinNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    }
}

bool BowlingPin::isKnockedDown() const {
    if (mPinNode) {
        // Une quille est considérée comme renversée si elle est inclinée de plus de 45 degrés
        Ogre::Vector3 upVector = mPinNode->getOrientation() * Ogre::Vector3::UNIT_Y;
        float dotProduct = upVector.dotProduct(Ogre::Vector3::UNIT_Y);
        Ogre::LogManager::getSingleton().logMessage("Pin " + mPinNode->getName() + " dotProduct: " + Ogre::StringConverter::toString(dotProduct));
        return dotProduct < 0.7071f;  // cos(45°) ≈ 0.7071
    }
    return false;
}

btRigidBody* BowlingPin::getPinBody() const {
    return mPinBody;
}

Ogre::SceneNode* BowlingPin::getPinNode() const {
    return mPinNode;
}