#include "../../include/objects/BowlingBall.h"

BowlingBall::BowlingBall(Ogre::SceneManager* sceneMgr, const Ogre::String& meshName)
    : mSceneMgr(sceneMgr),
      mBallNode(nullptr),
      mBallEntity(nullptr),
      mBallBody(nullptr),
      mRadius(0.108f),  
      mMass(7.0f),     
      mColor(Ogre::ColourValue(0.0f, 0.0f, 0.0f)), 
      mIsRolling(false),
      mInitialPosition(Ogre::Vector3::ZERO)
{}

BowlingBall::~BowlingBall() {
    if (mBallBody) {
        PhysicsManager::getInstance()->getDynamicsWorld()->getBtWorld()->removeRigidBody(mBallBody);
    }
}

void BowlingBall::create(const Ogre::Vector3& position) {
    auto physicsManager = PhysicsManager::getInstance();
    if (!physicsManager || !physicsManager->getDynamicsWorld()) {
        Ogre::LogManager::getSingleton().logError("BowlingBall::create - PhysicsManager ou DynamicsWorld non initialisé");
        return;
    }
    
    mInitialPosition = position;
    
    mBallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BowlingBallNode");
    mBallNode->setPosition(position);
    
    mBallEntity = mSceneMgr->createEntity("BowlingBallEntity", "ball.mesh");
    mBallNode->attachObject(mBallEntity);
    
    mBallNode->setScale(scale, scale, scale);
    
    mBallBody = physicsManager->getDynamicsWorld()->addRigidBody(
        mMass, mBallEntity, Ogre::Bullet::CT_SPHERE);
    
    if (mBallBody) {
        btCollisionObject* ballCollision = mBallBody;
        ballCollision->setFriction(0.5f);           
        ballCollision->setRestitution(0.1f);       
        ballCollision->setRollingFriction(0.1f);    // roulement
        ballCollision->setSpinningFriction(0.1f);   // rotation
        
        mBallBody->setActivationState(DISABLE_DEACTIVATION);
    }
}

void BowlingBall::reset() {
    if (mBallNode && mBallBody) {
        mBallNode->setPosition(mInitialPosition);
        mBallNode->setOrientation(Ogre::Quaternion::IDENTITY);
        Ogre::LogManager::getSingleton().logMessage("[BowlingBall::reset] Noeud Ogre réinitialisé à : " + 
            Ogre::StringConverter::toString(mInitialPosition));
        
        mBallBody->setLinearVelocity(btVector3(0, 0, 0));
        mBallBody->setAngularVelocity(btVector3(0, 0, 0));
        mBallBody->clearForces(); // Important pour annuler toute force appliquée

        btTransform initialTransform;
        initialTransform.setIdentity(); 
        initialTransform.setOrigin(btVector3(mInitialPosition.x, mInitialPosition.y, mInitialPosition.z));
        
        mBallBody->setWorldTransform(initialTransform);

        if (mBallBody->getMotionState()) {
            mBallBody->getMotionState()->setWorldTransform(initialTransform);
        }

        mBallBody->activate(true);

        Ogre::LogManager::getSingleton().logMessage("[BowlingBall::reset] Corps physique réinitialisé à : " + 
            Ogre::StringConverter::toString(mInitialPosition));

        mIsRolling = false;
    } else {
        Ogre::LogManager::getSingleton().logWarning("[BowlingBall::reset] Tentative de réinitialisation mais mBallNode ou mBallBody est nul.");
    }
}

void BowlingBall::launch(const Ogre::Vector3& direction, float power, float spin) {
    if (mBallBody) {
        Ogre::Vector3 normalizedDir = direction.normalisedCopy();
        
        // Application de la force linéaire (vitesse)
        btVector3 velocity(normalizedDir.x * power, 
                           normalizedDir.y * power, 
                           normalizedDir.z * power);
        mBallBody->setLinearVelocity(velocity);
        
        // Application de la rotation (effet)
        // L'effet est principalement une rotation autour de l'axe vertical (y)
        // mais peut aussi avoir une composante latérale (x)
        btVector3 angularVelocity(spin * 0.2f, spin, 0);
        mBallBody->setAngularVelocity(angularVelocity);
        
        // Mise à jour de l'état
        mIsRolling = true;
    }
}

void BowlingBall::update(float deltaTime) {
    if (mBallBody && mBallNode && mIsRolling) {
        // Mise à jour de la position du nœud Ogre à partir de la position du corps rigide Bullet
        btTransform transform;
        mBallBody->getMotionState()->getWorldTransform(transform);
        
        btVector3 position = transform.getOrigin();
        mBallNode->setPosition(position.x(), position.y(), position.z());
        
        // Mise à jour de la rotation du nœud Ogre à partir de la rotation du corps rigide Bullet
        btQuaternion rotation = transform.getRotation();
        mBallNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
        
        // Vérification si la boule s'est arrêtée
        btVector3 velocity = mBallBody->getLinearVelocity();
        btVector3 angularVelocity = mBallBody->getAngularVelocity();
        
        float linearSpeed = velocity.length();
        float angularSpeed = angularVelocity.length();
        
        if (linearSpeed < 0.01f && angularSpeed < 0.01f) {
            mIsRolling = false;
        }
    }
}

Ogre::Vector3 BowlingBall::getPosition() const {
    if (mBallNode) {
        return mBallNode->getPosition();
    }
    return Ogre::Vector3::ZERO;
}

Ogre::Vector3 BowlingBall::getVelocity() const {
    if (mBallBody) {
        btVector3 velocity = mBallBody->getLinearVelocity();
        return Ogre::Vector3(velocity.x(), velocity.y(), velocity.z());
    }
    return Ogre::Vector3::ZERO;
}

void BowlingBall::setColor(const Ogre::ColourValue& color) {
    mColor = color;
    
    // Mise à jour du matériau si l'entité existe déjà
    if (mBallEntity) {
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("BowlingBallMaterial");
        if (!material) {
            Ogre::Technique* technique = material->getTechnique(0);
            Ogre::Pass* pass = technique->getPass(0);
            pass->setDiffuse(mColor);
        }
    }
}

void BowlingBall::setMass(float mass) {
    mMass = mass;
    
    // Mise à jour de la masse si le corps rigide existe déjà
    if (mBallBody) {
        btVector3 inertia;
        btCollisionShape* shape = mBallBody->getCollisionShape();
        shape->calculateLocalInertia(mMass, inertia);
        mBallBody->setMassProps(mMass, inertia);
    }
}

void BowlingBall::setFriction(float friction) {
    if (mBallBody) {
        mBallBody->setFriction(friction);
    }
}

void BowlingBall::setRestitution(float restitution) {
    if (mBallBody) {
        mBallBody->setRestitution(restitution);
    }
}

void BowlingBall::setRollingFriction(float rollingFriction) {
    if (mBallBody) {
        mBallBody->setRollingFriction(rollingFriction);
    }
}

void BowlingBall::setSpinningFriction(float spinningFriction) {
    if (mBallBody) {
        mBallBody->setSpinningFriction(spinningFriction);
    }
}
