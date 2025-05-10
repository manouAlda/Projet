#include "../../Header/Bullet/DynamicsWorld.hpp"

namespace Bullet {

DynamicsWorld::DynamicsWorld(const Ogre::Vector3& gravity) {
    // Initialisation des composants de Bullet
    mCollisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
    mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfig.get());
    mBroadphase = std::make_unique<btDbvtBroadphase>();
    mSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
    
    // Création du monde dynamique
    mWorld = std::make_unique<btDiscreteDynamicsWorld>(
        mDispatcher.get(),
        mBroadphase.get(),
        mSolver.get(),
        mCollisionConfig.get()
    );
    
    // Configuration de la gravité
    mWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

DynamicsWorld::~DynamicsWorld() {
    // Supprimer tous les corps rigides
    for (auto& pair : mRigidBodies) {
        if (pair.second) {
            // Récupérer et supprimer le motion state
            btMotionState* motionState = pair.second->getMotionState();
            if (motionState) {
                delete motionState;
            }
            
            // Récupérer et supprimer la forme de collision
            btCollisionShape* shape = pair.second->getCollisionShape();
            if (shape) {
                delete shape;
            }
            
            // Suppression du monde
            mWorld->removeRigidBody(pair.second);
            
            // Libérer la mémoire
            delete pair.second;
        }
    }
    
    // Vider la map
    mRigidBodies.clear();
}

btRigidBody* DynamicsWorld::addRigidBody(float mass, Ogre::Entity* entity, CollisionType type) {
    if (!entity) {
        return nullptr;
    }
    
    // Création de la forme de collision en fonction du type
    btCollisionShape* shape = nullptr;
    
    // Récupérer les informations de taille de l'entité
    const Ogre::AxisAlignedBox& aabb = entity->getBoundingBox();
    Ogre::Vector3 size = aabb.getSize();
    
    switch (type) {
        case CT_BOX:
            shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
            break;
        case CT_SPHERE: {
            float radius = std::max(std::max(size.x, size.y), size.z) * 0.5f;
            shape = new btSphereShape(radius);
            break;
        }
        case CT_CYLINDER:
            shape = new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
            break;
        case CT_CAPSULE: {
            float radius = std::max(size.x, size.z) * 0.5f;
            float height = size.y - 2 * radius;
            shape = new btCapsuleShape(radius, height);
            break;
        }
        case CT_CONE: {
            float radius = std::max(size.x, size.z) * 0.5f;
            shape = new btConeShape(radius, size.y);
            break;
        }
        case CT_TRIMESH: {
            // Pour les maillages complexes comme le terrain
            // Cette implémentation est simplifiée et peut nécessiter plus de travail
            // pour les maillages réels
            btTriangleMesh* triangleMesh = new btTriangleMesh();
            
            // Ajouter les sommets (ceci est une simulation, le vrai code doit parcourir tous les triangles)
            triangleMesh->addTriangle(
                btVector3(-size.x * 0.5f, 0, -size.z * 0.5f),
                btVector3(size.x * 0.5f, 0, -size.z * 0.5f),
                btVector3(0, 0, size.z * 0.5f)
            );
            
            shape = new btBvhTriangleMeshShape(triangleMesh, true);
            break;
        }
        case CT_HULL: {
            // Pas implémenté ici
            shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
            break;
        }
        default:
            shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    
    if (!shape) {
        return nullptr;
    }
    
    // Position et orientation initiales
    Ogre::SceneNode* node = entity->getParentSceneNode();
    btTransform startTransform;
    startTransform.setIdentity();
    
    if (node) {
        Ogre::Vector3 pos = node->getPosition();
        Ogre::Quaternion rot = node->getOrientation();
        
        startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
    }
    
    // Création du motion state
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    
    // Calcul de l'inertie
    btVector3 localInertia(0, 0, 0);
    if (mass > 0) {
        shape->calculateLocalInertia(mass, localInertia);
    }
    
    // Configuration du corps rigide
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    
    // Ajout au monde physique
    mWorld->addRigidBody(body);
    
    // Stockage du corps rigide
    mRigidBodies[entity] = body;
    
    // Stocker un pointeur vers le nœud pour les mises à jour
    body->setUserPointer(node);
    
    return body;
}

void DynamicsWorld::stepSimulation(float timeStep, int maxSubSteps) {
    // Avancer la simulation physique
    mWorld->stepSimulation(timeStep, maxSubSteps);
    
    // Mettre à jour les positions des objets visuels
    for (auto& pair : mRigidBodies) {
        Ogre::MovableObject* movable = pair.first;
        btRigidBody* body = pair.second;
        
        if (movable && body && body->getMotionState() && body->getUserPointer()) {
            Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(body->getUserPointer());
            
            // Récupérer la transformation actuelle du corps rigide
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            
            // Appliquer la position au nœud de scène
            btVector3 pos = trans.getOrigin();
            node->setPosition(pos.x(), pos.y(), pos.z());
            
            // Appliquer la rotation au nœud de scène
            btQuaternion rot = trans.getRotation();
            node->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        }
    }
}

void DynamicsWorld::removeRigidBody(btRigidBody* body) {
    if (body) {
        // Supprimer du monde
        mWorld->removeRigidBody(body);
        
        // Supprimer de notre map
        for (auto it = mRigidBodies.begin(); it != mRigidBodies.end(); ++it) {
            if (it->second == body) {
                mRigidBodies.erase(it);
                break;
            }
        }
        
        // Libérer la mémoire
        btMotionState* motionState = body->getMotionState();
        if (motionState) {
            delete motionState;
        }
        
        btCollisionShape* shape = body->getCollisionShape();
        if (shape) {
            delete shape;
        }
        
        delete body;
    }
}

void DynamicsWorld::removeRigidBody(Ogre::MovableObject* movable) {
    if (movable && mRigidBodies.find(movable) != mRigidBodies.end()) {
        removeRigidBody(mRigidBodies[movable]);
        mRigidBodies.erase(movable);
    }
}

} // namespace Bullet