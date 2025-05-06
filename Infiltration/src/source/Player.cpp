// Player.cpp - Implémentation de la classe Player
#include "Player.h"
#include "GameManager.h"
#include "Shapes/OgreBulletCollisionsCapsuleShape.h"

Player::Player(GameManager* gameManager, const Ogre::String& name, const Ogre::Vector3& position)
    : mGameManager(gameManager)
    , mName(name)
    , mWalkSpeed(5.0f)
    , mRunSpeed(10.0f)
    , mCurrentSpeed(mWalkSpeed)
    , mTurnSpeed(0.2f)
    , mJumpForce(10.0f)
    , mStealthLevel(0.0f)
    , mIsRunning(false)
    , mIsVisible(true)
    , mIsOnGround(true)
    , mMoveForward(false)
    , mMoveBackward(false)
    , mMoveLeft(false)
    , mMoveRight(false)
    , mJump(false)
    , mIsCrouching(false)
    , mCamera(nullptr)
    , mCameraNode(nullptr)
    , mCameraHeight(1.8f)
    , mCameraPitch(0.0f)
    , mCameraYaw(0.0f)
    , mState(IDLE)
{
    // Créer l'entité et le nœud
    Ogre::SceneManager* sceneMgr = mGameManager->getSceneManager();
    mEntity = sceneMgr->createEntity(mName, "ninja.mesh");  // Utiliser un modèle adapté
    mNode = sceneMgr->getRootSceneNode()->createChildSceneNode(mName + "_node", position);
    mNode->attachObject(mEntity);
    mNode->setScale(0.3f, 0.3f, 0.3f);  // Adapter la taille
    
    // Créer le corps physique
    createPhysicsBody();
    
    // Créer le nœud de caméra (sera attaché plus tard)
    mCameraNode = mNode->createChildSceneNode(mName + "_camera_node", Ogre::Vector3(0, mCameraHeight, 0));
}

Player::~Player()
{
    // Le nettoyage des corps physiques est géré par GameManager
}

void Player::createPhysicsBody()
{
    // Créer une forme de capsule pour le joueur
    float height = 1.8f;
    float radius = 0.4f;
    OgreBulletCollisions::CapsuleCollisionShape* capsuleShape = 
        new OgreBulletCollisions::CapsuleCollisionShape(radius, height, Ogre::Vector3::UNIT_Y);
    
    // Créer le corps physique
    float mass = 80.0f;  // masse en kg
    mBody = mGameManager->createRigidBody(mName + "_body", mass, mNode->getPosition(), mNode->getOrientation(), capsuleShape);
    
    // Définir les paramètres physiques
    mBody->setLinearFactor(Ogre::Vector3(1, 1, 1));  // Autoriser le mouvement dans toutes les directions
    mBody->setAngularFactor(Ogre::Vector3(0, 1, 0));  // Limiter la rotation à l'axe Y
    mBody->disableDeactivation();  // Toujours actif
    mBody->setFriction(0.7f);
    mBody->setDamping(0.1f, 0.1f);  // Amortissement linéaire et angulaire
}

void Player::update(float deltaTime)
{
    // Mettre à jour l'état
    updateState();
    
    // Mettre à jour le mouvement
    updateMovement(deltaTime);
    
    // Mettre à jour la caméra
    updateCamera(deltaTime);
    
    // Mettre à jour le niveau de furtivité
    updateStealthLevel();
    
    // Synchroniser la position du nœud avec le corps physique
    if (mBody) {
        mNode->setPosition(mBody->getWorldPosition());
        mNode->setOrientation(mBody->getWorldOrientation());
    }
}

void Player::updateState()
{
    // Déterminer l'état du joueur
    if (mIsRunning && (mMoveForward || mMoveBackward || mMoveLeft || mMoveRight)) {
        mState = RUNNING;
        mCurrentSpeed = mRunSpeed;
        mIsVisible = true;  // Plus visible en courant
    }
    else if (mIsCrouching) {
        mState = CROUCHING;
        mCurrentSpeed = mWalkSpeed * 0.5f;
        mIsVisible = false;  // Moins visible en s'accroupissant
    }
    else if (mMoveForward || mMoveBackward || mMoveLeft || mMoveRight) {
        mState = WALKING;
        mCurrentSpeed = mWalkSpeed;
        mIsVisible = true;  // Visible en marchant
    }
    else {
        mState = IDLE;
        mIsVisible = false;  // Moins visible en restant immobile
    }
    
    // Si le joueur saute
    if (!mIsOnGround) {
        mState = JUMPING;
    }
}

void Player::updateMovement(float deltaTime)
{
    // Calculer la direction du mouvement
    Ogre::Vector3 direction = Ogre::Vector3::ZERO;
    
    if (mMoveForward) direction += Ogre::Vector3(0, 0, -1);
    if (mMoveBackward) direction += Ogre::Vector3(0, 0, 1);
    if (mMoveLeft) direction += Ogre::Vector3(-1, 0, 0);
    if (mMoveRight) direction += Ogre::Vector3(1, 0, 0);
    
    // Normaliser la direction si nécessaire
    if (direction.squaredLength() > 0.001f) {
        direction.normalise();
        
        // Transformer la direction selon l'orientation du joueur
        direction = mNode->getOrientation() * direction;
        
        // Appliquer le mouvement
        move(direction, deltaTime);
    }
    else {
        // Réduire la vitesse si aucune touche n'est enfoncée
        if (mBody) {
            Ogre::Vector3 currentVel = mBody->getLinearVelocity();
            currentVel.x *= 0.9f;
            currentVel.z *= 0.9f;
            mBody->setLinearVelocity(currentVel);
        }
    }
    
    // Gérer le saut
    if (mJump && mIsOnGround) {
        jump();
        mJump = false;
    }
    
    // Vérifier si le joueur est au sol
    checkGroundContact();
}

void Player::updateCamera(float deltaTime)
{
    if (mCameraNode && mCamera) {
        // Mettre à jour la position de la caméra
        float height = mIsCrouching ? mCameraHeight * 0.6f : mCameraHeight;
        mCameraNode->setPosition(0, height, 0);
        
        // Appliquer les rotations de la caméra
        mCameraNode->setOrientation(
            Ogre::Quaternion(Ogre::Degree(mCameraYaw), Ogre::Vector3::UNIT_Y) *
            Ogre::Quaternion(Ogre::Degree(mCameraPitch), Ogre::Vector3::UNIT_X)
        );
        
        // Le regard pointe dans le -Z de la caméra
        mCamera->setOrientation(mCameraNode->getOrientation());
    }
}

void Player::updateStealthLevel()
{
    // Le niveau de furtivité est une combinaison de plusieurs facteurs
    mStealthLevel = 0.0f;
    
    // Plus élevé = plus visible
    if (mIsRunning) {
        mStealthLevel += 1.0f;
    }
    else if (mState == WALKING) {
        mStealthLevel += 0.5f;
    }
    else if (mState == IDLE) {
        mStealthLevel += 0.1f;
    }
    
    // Réduire la visibilité en s'accroupissant
    if (mIsCrouching) {
        mStealthLevel *= 0.5f;
    }
    
    // Limiter entre 0 et 1
    if (mStealthLevel < 0.0f) mStealthLevel = 0.0f;
    if (mStealthLevel > 1.0f) mStealthLevel = 1.0f;
}

void Player::checkGroundContact()
{
    // Vérifier si le joueur est au sol en faisant un raycast vers le bas
    Ogre::Vector3 from = mNode->getPosition();
    Ogre::Vector3 to = from - Ogre::Vector3(0, 0.1f, 0);
    
    OgreBulletCollisions::CollisionClosestRayResultCallback rayCallback(from, to);
    mGameManager->getDynamicsWorld()->launchRay(rayCallback);
    
    mIsOnGround = rayCallback.hasHit();
}

void Player::move(const Ogre::Vector3& direction, float deltaTime)
{
    if (mBody) {
        // Obtenir la vitesse actuelle
        Ogre::Vector3 velocity = mBody->getLinearVelocity();
        
        // Appliquer la force de mouvement
        Ogre::Vector3 moveForce = direction * mCurrentSpeed * 100.0f;
        mBody->applyCentralForce(moveForce);
        
        // Limiter la vitesse horizontale
        float maxSpeed = mCurrentSpeed;
        float currentSpeedXZ = Ogre::Math::Sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
        
        if (currentSpeedXZ > maxSpeed) {
            float scale = maxSpeed / currentSpeedXZ;
            velocity.x *= scale;
            velocity.z *= scale;
            mBody->setLinearVelocity(velocity);
        }
    }
}

void Player::rotate(float yaw, float pitch)
{
    // Mettre à jour l'orientation de la caméra
    mCameraYaw += yaw;
    mCameraPitch += pitch;
    
    // Limiter le pitch pour éviter de regarder trop haut ou trop bas
    if (mCameraPitch > 80.0f) mCameraPitch = 80.0f;
    if (mCameraPitch < -80.0f) mCameraPitch = -80.0f;
    
    // Le corps du joueur suit la rotation de la caméra uniquement sur l'axe Y
    if (mBody) {
        Ogre::Quaternion orientation = Ogre::Quaternion(Ogre::Degree(mCameraYaw), Ogre::Vector3::UNIT_Y);
        mBody->setOrientation(orientation);
    }
}

void Player::jump()
{
    if (mBody && mIsOnGround) {
        // Appliquer une impulsion vers le haut
        mBody->applyCentralImpulse(Ogre::Vector3(0, mJumpForce, 0));
        mIsOnGround = false;
    }
}

void Player::crouch(bool isCrouching)
{
    mIsCrouching = isCrouching;
    
    // Ajuster la hauteur de la capsule physique
    // TODO: Ajuster la collision si nécessaire
}

void Player::run(bool isRunning)
{
    mIsRunning = isRunning;
}

void Player::interact()
{
    // Raycast pour détecter les objets interactifs
    Ogre::Vector3 from = mCameraNode->_getDerivedPosition();
    Ogre::Vector3 direction = mCameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
    Ogre::Vector3 to = from + direction * 2.0f;  // Distance d'interaction
    
    OgreBulletCollisions::CollisionClosestRayResultCallback rayCallback(from, to);
    mGameManager->getDynamicsWorld()->launchRay(rayCallback);
    
    if (rayCallback.hasHit()) {
        // Obtenir l'objet touché
        OgreBulletCollisions::Object* object = rayCallback.getCollidedObject();
        
        // TODO: Implémenter l'interaction avec l'objet
        std::cout << "Interaction avec " << object->getName() << std::endl;
    }
}

void Player::attachCamera(Ogre::Camera* camera)
{
    mCamera = camera;
    
    if (mCamera && mCameraNode) {
        // Détacher la caméra de son nœud parent s'il en a un
        if (mCamera->getParentSceneNode()) {
            mCamera->getParentSceneNode()->detachObject(mCamera);
        }
        
        // Attacher la caméra au nœud de caméra du joueur
        mCameraNode->attachObject(mCamera);
    }
}

Ogre::Vector3 Player::getPosition() const
{
    if (mNode) {
        return mNode->getPosition();
    }
    return Ogre::Vector3::ZERO;
}

Ogre::Quaternion Player::getOrientation() const
{
    if (mNode) {
        return mNode->getOrientation();
    }
    return Ogre::Quaternion::IDENTITY;
}

void Player::keyPressed(const OIS::KeyEvent& evt)
{
    switch (evt.key) {
        case OIS::KC_W:
            mMoveForward = true;
            break;
        case OIS::KC_S:
            mMoveBackward = true;
            break;
        case OIS::KC_A:
            mMoveLeft = true;
            break;
        case OIS::KC_D:
            mMoveRight = true;
            break;
        case OIS::KC_SPACE:
            mJump = true;
            break;
        case OIS::KC_LCONTROL:
        case OIS::KC_RCONTROL:
            crouch(true);
            break;
        case OIS::KC_LSHIFT:
        case OIS::KC_RSHIFT:
            run(true);
            break;
        case OIS::KC_E:
            interact();
            break;
    }
}

void Player::keyReleased(const OIS::KeyEvent& evt)
{
    switch (evt.key) {
        case OIS::KC_W:
            mMoveForward = false;
            break;
        case OIS::KC_S:
            mMoveBackward = false;
            break;
        case OIS::KC_A:
            mMoveLeft = false;
            break;
        case OIS::KC_D:
            mMoveRight = false;
            break;
        case OIS::KC_LCONTROL:
        case OIS::KC_RCONTROL:
            crouch(false);
            break;
        case OIS::KC_LSHIFT:
        case OIS::KC_RSHIFT:
            run(false);
            break;
    }
}

void Player::mouseMoved(const OIS::MouseEvent& evt)
{
    // Sensibilité de la souris
    float sensitivity = 0.2f;
    
    // Mise à jour de l'orientation
    rotate(-evt.state.X.rel * sensitivity, -evt.state.Y.rel * sensitivity);
}

void Player::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    // Pour une éventuelle interaction avec le clic
    if (id == OIS::MB_Left) {
        interact();
    }
}

void Player::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    // Non utilisé pour l'instant
}