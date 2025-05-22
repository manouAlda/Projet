#include "../../include/objects/ObjectFactory.h"

btRigidBody* ObjectFactory::createStaticGround(){
    // Création du plan dans Ogre
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 1000, 1000, 10, 10, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
    
    // Création de l'entité et du nœud
    Ogre::Entity* groundEntity = mSceneMgr->createEntity("GroundEntity", "ground");
    
    // Création d'un matériau simple pour le sol
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        "GroundMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Technique* technique = material->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    pass->setDiffuse(Ogre::ColourValue(1, 0, 0));  // gris
    pass->setSpecular(Ogre::ColourValue(0.1, 0.1, 0.1));
    pass->setShininess(30);
    
    groundEntity->setMaterialName("GroundMaterial");
    
    Ogre::SceneNode* groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode");
    groundNode->attachObject(groundEntity);
    
    // Ajout du sol au monde physique (masse 0 = objet statique)
    return PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(0.0f, groundEntity, Ogre::Bullet::CT_BOX);
}

btRigidBody* ObjectFactory::createDynamicBox(const Ogre::Vector3& position, const Ogre::String& name, float mass){
    // Création de l'entité et du nœud dans Ogre
    Ogre::Entity* boxEntity = mSceneMgr->createEntity(name, "cube.mesh");
    Ogre::SceneNode* boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name + "Node");
    boxNode->attachObject(boxEntity);
    boxNode->setPosition(position);
    
    // Ajout du cube au monde physique avec une masse > 0 (objet dynamique)
    return PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(mass, boxEntity, Ogre::Bullet::CT_BOX);
}

btRigidBody* ObjectFactory::createDynamicSphere(const Ogre::Vector3& position, const Ogre::String& name, float mass){
    // Création de l'entité et du nœud dans Ogre
    Ogre::Entity* sphereEntity = mSceneMgr->createEntity(name, "sphere.mesh");
    Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name + "Node");
    sphereNode->attachObject(sphereEntity);
    sphereNode->setPosition(position);
    
    // Ajout de la sphère au monde physique
    return PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(mass, sphereEntity, Ogre::Bullet::CT_SPHERE);
}

btRigidBody* ObjectFactory::createDynamicCylinder(const Ogre::Vector3& position, const Ogre::String& name, float mass){
    // Création de l'entité et du nœud dans Ogre
    Ogre::Entity* cylinderEntity = mSceneMgr->createEntity(name, "spine.mesh");
    Ogre::SceneNode* cylinderNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name + "Node");
    cylinderNode->attachObject(cylinderEntity);
    cylinderNode->setPosition(position);
    
    // Ajout du cylindre au monde physique
    return PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(mass, cylinderEntity, Ogre::Bullet::CT_CYLINDER);
}

void ObjectFactory::createStack(int height, const Ogre::Vector3& position){
    float boxSize = 1.0f;
    float spacing = boxSize * 1.1f;
    
    for (int i = 0; i < height; i++)
    {
        Ogre::String name = "StackBox" + Ogre::StringConverter::toString(i);
        Ogre::Vector3 pos = position + Ogre::Vector3(0, boxSize/2 + i * spacing, 0);
        createDynamicBox(pos, name, 1.0f);
    }
}

