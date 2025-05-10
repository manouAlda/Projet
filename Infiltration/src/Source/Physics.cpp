#include "../Header/Physics.hpp"
#include "../Header/Plane.hpp" 

Physics::Physics(Ogre::SceneManager* sceneMgr) : sceneMgr(sceneMgr), mPlane(nullptr) {
    // Initialisation du monde dynamique avec la gravité
    world = std::make_unique<Bullet::DynamicsWorld>(Ogre::Vector3(0, -9.81, 0));
}

Physics::~Physics() {
    // La destruction est gérée par les destructeurs des smart pointers
}

void Physics::update(float deltaTime) {
    world->stepSimulation(deltaTime, 10);
    
    if (debugDrawer) {
        debugDrawer->update();
    }
}

Bullet::DynamicsWorld* Physics::getDynamicsWorld() {
    return world.get();
}

void Physics::createStaticGround(Physics* physics, SceneManager* sceneMgr) {
    // Plane
    // Commentez cette partie si la classe Plane n'existe pas encore
    
    mPlane = new ::Plane(sceneMgr, "BackgroundMaterial"); 
    sceneMgr->setSkyBox(true, "Ciel");
    
    // Si vous n'avez pas de classe Plane, utilisez un simple plan d'Ogre
    // Pour le skybox, vérifiez que "Ciel" existe dans vos ressources, sinon commentez la ligne
    
    // Creation d'un sol statique
/*    
    Entity* entity = sceneMgr->createEntity("Ground", "export.mesh");
    
    SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode("nodeGround");
    node->attachObject(entity);
    node->setPosition(0, 0, 0);
    
    //node->roll(Ogre::Degree(180));
    node->setScale(15, 15, 15);
    //node->yaw(Ogre::Degree(-90));
    
    // Ajout du sol au monde physics (masse = 0 = objet statique)
    btRigidBody* groundBody = physics->getDynamicsWorld()->addRigidBody(0, entity, Bullet::CT_TRIMESH);
    
    // DebugDrawer
    debugNode = sceneMgr->getRootSceneNode()->createChildSceneNode("DebugNode");
    debugDrawer = std::make_unique<Bullet::DebugDrawer>(debugNode, physics->getBtWorld());
*/
}

btRigidBody* Physics::createDynamicCube(Physics* physics, Ogre::SceneManager* sceneMgr, const Ogre::Vector3& position, const Ogre::String& name, float mass) {
    // Création de l'entité et du nœud dans Ogre
    Entity* cubeEntity = sceneMgr->createEntity(name, "Cube.mesh");
    SceneNode* cubeNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name + "Node");
    cubeNode->attachObject(cubeEntity);
    cubeNode->setPosition(position);
    
    // Ajout du cube au monde physique avec une masse > 0 (objet dynamique)
    return physics->getDynamicsWorld()->addRigidBody(mass, cubeEntity, Bullet::CT_BOX);
}

btRigidBody* Physics::createDynamicSphere(Physics* physics, Ogre::SceneManager* sceneMgr, const Ogre::Vector3& position, const Ogre::String& name, float mass) {
    // Création de l'entité et du nœud dans Ogre
    Entity* sphereEntity = sceneMgr->createEntity(name, "spheree.mesh");
    SceneNode* sphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name + "Node");
    sphereNode->attachObject(sphereEntity);
    sphereNode->setPosition(position);
    
    // Ajout de la sphère au monde physique
    return physics->getDynamicsWorld()->addRigidBody(mass, sphereEntity, Bullet::CT_SPHERE);
}

btRigidBody* Physics::createDynamicCharacter(Physics* physics, Ogre::SceneManager* sceneMgr, const Ogre::Vector3& position, const Ogre::String& name, float mass) {
    // Création de l'entité et du nœud dans Ogre
    Entity* characterEntity = sceneMgr->createEntity(name, "Mesh.mesh"); 
    SceneNode* characterNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name + "Node");
    characterNode->attachObject(characterEntity);
    characterNode->setPosition(position);
    
    // Ajout du personnage au monde physique
    return physics->getDynamicsWorld()->addRigidBody(mass, characterEntity, Bullet::CT_CAPSULE); // Utilisez une capsule pour représenter le personnage
}