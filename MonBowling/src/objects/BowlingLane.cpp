#include "../../include/objects/BowlingLane.h"

BowlingLane::BowlingLane(Ogre::SceneManager* sceneMgr)
    : sceneMgr(sceneMgr),
      laneNode(nullptr),
      laneEntity(nullptr){
    // Initialisation des quilles avec une taille standard de 10
    pins.resize(10);
}

BowlingLane::~BowlingLane() {}

void createGround(Ogre::SceneManager* sceneMgr) {
    Ogre::Entity* mPlaneEnt;
    Ogre::SceneNode* mPlaneNode;

    // Création d'un plane
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "ground", Ogre::RGN_DEFAULT,
        plane,
        1500, 1500, 20, 20,
        true,
        1, 5, 5,
        Ogre::Vector3::UNIT_Z);

    //creation de l'entite
    mPlaneEnt = sceneMgr->createEntity("ground");
    mPlaneEnt->setMaterialName("BackgroundMaterial");
    mPlaneEnt->setCastShadows(false);

    //creation du noeud
    mPlaneNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    mPlaneNode->attachObject(mPlaneEnt);
    mPlaneNode->setPosition(0, -2.5, 0);

    // mettre le plane dynamic
    btRigidBody* groundBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, mPlaneEnt, Ogre::Bullet::CT_BOX);

    sceneMgr->setSkyBox(true, "Ciel", 5000);
}

void BowlingLane::create(const Ogre::Vector3& ballStart) {
    // On sauvegarde la position de départ de la boule pour positionner correctement la piste
    ballStartPosition = ballStart;
    
    createGround(sceneMgr);

    // Création de la piste
    createLane();
    
    // Création et positionnement des quilles
    setupPins();
}

void BowlingLane::createLane() {
    // Création du nœud pour la piste
    laneNode = sceneMgr->getRootSceneNode()->createChildSceneNode("BowlingLaneNode");
    
    laneEntity = sceneMgr->createEntity("LaneEntity", "polygon8.mesh");
    laneNode->attachObject(laneEntity);
    //laneNode->yaw(Ogre::Radian(Ogre::Degree(90)));  // lane2.mesh
    laneNode->yaw(Ogre::Degree(180));   /// Garry (polygon8.mesh)
    
    // Positionnement de la piste
    laneNode->setPosition(0.0f, 0.0f, 0.0f); 
    laneNode->setScale(10.0f, 10.0f, 10.0f);
    
    // Ajout de la piste au monde physique comme objet statique
    btRigidBody* laneBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, laneEntity, Ogre::Bullet::CT_TRIMESH);
    
    if (laneBody) { laneBody->setFriction(0.3f); }

    Ogre::LogManager::getSingleton().logMessage("Piste à : " + Ogre::StringConverter::toString(laneNode->getPosition()));
}

void BowlingLane::setupPins() {
    float pinZOffset = 9.0f; 
    float pinXOffset = 0.0f; 
    Ogre::Vector3 pinBasePosition = Ogre::Vector3(
        ballStartPosition.x + pinXOffset,    
        0.0f,                   
        ballStartPosition.z + pinZOffset  
    );
    
    // Espacement entre les quilles
    float spacing = 0.05f;
    
    // Position des quilles en formation triangulaire standard
    // Rangée 1 (la plus éloignée de la boule, une seule quille)
    Ogre::Vector3 pinPositions[10];
    pinPositions[0] = pinBasePosition;
    
    // Rangée 2 (deux quilles, plus près de la boule)
    pinPositions[1] = pinBasePosition + Ogre::Vector3(-spacing, 0, spacing);
    pinPositions[2] = pinBasePosition + Ogre::Vector3(spacing, 0, spacing);
    
    // Rangée 3 (trois quilles)
    pinPositions[3] = pinBasePosition + Ogre::Vector3(-2*spacing, 0, 2*spacing);
    pinPositions[4] = pinBasePosition + Ogre::Vector3(0, 0, 2*spacing);
    pinPositions[5] = pinBasePosition + Ogre::Vector3(2*spacing, 0, 2*spacing);
    
    // Rangée 4 (quatre quilles, la plus proche de la boule)
    pinPositions[6] = pinBasePosition + Ogre::Vector3(-3*spacing, 0, 3*spacing);
    pinPositions[7] = pinBasePosition + Ogre::Vector3(-spacing, 0, 3*spacing);
    pinPositions[8] = pinBasePosition + Ogre::Vector3(spacing, 0, 3*spacing);
    pinPositions[9] = pinBasePosition + Ogre::Vector3(3*spacing, 0, 3*spacing);
    
    // Création des quilles
    for (int i = 0; i < 10; ++i) {
        pins[i] = std::make_unique<BowlingPin>(sceneMgr);
        pins[i]->create(pinPositions[i], i+1);
        Ogre::LogManager::getSingleton().logMessage("Quille " + Ogre::StringConverter::toString(i+1) + 
                                                    " à : " + Ogre::StringConverter::toString(pinPositions[i]));
    }
    
    pinsInitialized = true;
}

const std::vector<std::unique_ptr<BowlingPin>>& BowlingLane::getPins() const {
    return pins;
}

void BowlingLane::update(float deltaTime) {
    if (pinsInitialized) {
        for (auto& pin : pins) {
            if (pin) {
                pin->update(deltaTime);
            }
        }
    }
}

void BowlingLane::resetPins() {
    if (pinsInitialized) {
        for (auto& pin : pins) {
            if (pin) {
                pin->reset();
            }
        }
    }
}

int BowlingLane::countKnockedDownPins() const {
    int knockedDownCount = 0;
    
    if (pinsInitialized) {
        for (const auto& pin : pins) {
            if (pin && pin->isKnockedDown()) {
                knockedDownCount++;
            }
        }
    }
    
    return knockedDownCount;
}