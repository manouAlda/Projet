#include "../Header/Plane.hpp"
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btDefaultMotionState.h>

/**
 * Constructeur qui crée un plan pour le sol de la scène
 * @param sceneMgr Le gestionnaire de scène Ogre
 * @param materialName Le nom du matériau à appliquer au plan
 * @param dynamicsWorld Le monde dynamique de Bullet (optionnel)
 */
Plane::Plane(Ogre::SceneManager* sceneMgr, const Ogre::String& materialName, btDiscreteDynamicsWorld* dynamicsWorld) 
    : mSceneMgr(sceneMgr), mDynamicsWorld(dynamicsWorld), mRigidBody(nullptr), mCollisionShape(nullptr) {
    // Création du plan
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "ground", 
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 
        5000, 5000, 20, 20, 
        true, 
        1, 5, 5, 
        Ogre::Vector3::UNIT_Z
    );
    
    // Création de l'entité à partir du mesh
    mPlaneEntity = mSceneMgr->createEntity("GroundEntity", "ground");
    
    // Application du matériau
    mPlaneEntity->setMaterialName(materialName);
    
    // Création du noeud de scène et attachement de l'entité
    mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode");
    mPlaneNode->attachObject(mPlaneEntity);
    
    // Réglage de la position et de l'échelle
    mPlaneNode->setPosition(0, 0, 0);
    // Si nécessaire, ajustez l'échelle
    // mPlaneNode->setScale(1, 1, 1);
    
    // Si un monde dynamique a été fourni, on crée un corps rigide pour le plan
    if (mDynamicsWorld) {
        // Créer une forme de collision pour le plan
        mCollisionShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
        
        // Configuration du corps rigide
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, 0, 0));
        
        // État de mouvement (pour un plan statique)
        btDefaultMotionState* motionState = new btDefaultMotionState(groundTransform);
        
        // Info de construction du corps rigide
        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, mCollisionShape, btVector3(0, 0, 0));
        
        // Création du corps rigide
        mRigidBody = new btRigidBody(rbInfo);
        
        // Définir les propriétés de friction
        mRigidBody->setFriction(0.8f);
        mRigidBody->setRollingFriction(0.1f);
        mRigidBody->setRestitution(0.1f);  // Rebond
        
        // Ajouter le corps rigide au monde
        mDynamicsWorld->addRigidBody(mRigidBody);
    }
}

/**
 * Destructeur
 */
Plane::~Plane() {
    // Nettoyer les ressources physiques
    if (mDynamicsWorld && mRigidBody) {
        mDynamicsWorld->removeRigidBody(mRigidBody);
        
        // Supprimer l'état de mouvement
        if (mRigidBody->getMotionState()) {
            delete mRigidBody->getMotionState();
        }
        
        delete mRigidBody;
        mRigidBody = nullptr;
    }
    
    // Supprimer la forme de collision
    if (mCollisionShape) {
        delete mCollisionShape;
        mCollisionShape = nullptr;
    }
    
    // La suppression des entités et noeuds est gérée par Ogre
    // Mais nous pouvons nettoyer explicitement si nécessaire
    if (mPlaneNode) {
        mPlaneNode->detachAllObjects();
        mSceneMgr->destroySceneNode(mPlaneNode);
    }
    
    if (mPlaneEntity) {
        mSceneMgr->destroyEntity(mPlaneEntity);
    }
    
    // Supprimer le mesh plane
    Ogre::MeshManager::getSingleton().remove("ground");
}

/**
 * Récupère l'entité du plan
 * @return L'entité du plan
 */
Ogre::Entity* Plane::getEntity() const {
    return mPlaneEntity;
}

/**
 * Récupère le noeud de scène du plan
 * @return Le noeud de scène du plan
 */
Ogre::SceneNode* Plane::getSceneNode() const {
    return mPlaneNode;
}

/**
 * Définit la position du plan
 * @param position La nouvelle position
 */
void Plane::setPosition(const Ogre::Vector3& position) {
    mPlaneNode->setPosition(position);
}

/**
 * Définit la rotation du plan
 * @param rotation La rotation à appliquer en quaternion
 */
void Plane::setRotation(const Ogre::Quaternion& rotation) {
    mPlaneNode->setOrientation(rotation);
}

/**
 * Définit le matériau du plan
 * @param materialName Le nom du matériau à appliquer
 */
void Plane::setMaterial(const Ogre::String& materialName) {
    mPlaneEntity->setMaterialName(materialName);
}

/**
 * Définit les propriétés physiques du plan
 * @param friction Coefficient de friction
 * @param restitution Coefficient de restitution (rebond)
 */
void Plane::setPhysicsProperties(float friction, float restitution) {
    if (mRigidBody) {
        mRigidBody->setFriction(friction);
        mRigidBody->setRestitution(restitution);
    }
}