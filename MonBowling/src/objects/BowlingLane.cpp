#include "../../include/objects/BowlingLane.h"

BowlingLane::BowlingLane(Ogre::SceneManager* sceneMgr)
    : mSceneMgr(sceneMgr),
      mLaneNode(nullptr),
      mLaneEntity(nullptr){
    // Initialisation des quilles avec une taille standard de 10
    mPins.resize(10);
}

BowlingLane::~BowlingLane() {}

void BowlingLane::create(const Ogre::Vector3& ballStartPosition) {
    // On sauvegarde la position de départ de la boule pour positionner correctement la piste
    mBallStartPosition = ballStartPosition;
    
    // Création de la piste
    createLane();
    
    // Création et positionnement des quilles
    setupPins();
}

void BowlingLane::createLane() {
    // Création du nœud pour la piste
    mLaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BowlingLaneNode");
    
    mLaneEntity = mSceneMgr->createEntity("LaneEntity", "lane2.mesh");
    mLaneNode->attachObject(mLaneEntity);
    mLaneNode->yaw(Ogre::Radian(Ogre::Degree(90))); 
    
    // Positionnement de la piste
    mLaneNode->setPosition(0.0f, 0.0f, 0.0f); 

    // Ajout d'une mise à l'échelle pour agrandir le modèle
    mLaneNode->setScale(10.0f, 10.0f, 10.0f);
    
    // Ajout de la piste au monde physique comme objet statique
    btRigidBody* laneBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, mLaneEntity, Ogre::Bullet::CT_TRIMESH);
    
    if (laneBody) { laneBody->setFriction(0.3f); }

    Ogre::LogManager::getSingleton().logMessage("Piste à : " + Ogre::StringConverter::toString(mLaneNode->getPosition()));
}

void BowlingLane::setupPins() {
    // Calculer la position des quilles directement par rapport à mBallStartPosition
    float pinZOffset = 100.0f; // 95 unités derrière la boule (z négatif)
    float pinXOffset = -3.0f; // Pas de décalage sur l'axe X
    Ogre::Vector3 pinBasePosition = Ogre::Vector3(
        mBallStartPosition.x + pinXOffset,    
        1.5f,                   
        mBallStartPosition.z + pinZOffset  
    );
    
    // Espacement entre les quilles
    float spacing = 1.0f;
    
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
        mPins[i] = std::make_unique<BowlingPin>(mSceneMgr);
        mPins[i]->create(pinPositions[i], i+1);
        Ogre::LogManager::getSingleton().logMessage("Quille " + Ogre::StringConverter::toString(i+1) + 
                                                    " à : " + Ogre::StringConverter::toString(pinPositions[i]));
    }
    
    mPinsInitialized = true;
}

void BowlingLane::update(float deltaTime) {
    if (mPinsInitialized) {
        for (auto& pin : mPins) {
            if (pin) {
                pin->update(deltaTime);
            }
        }
    }
}

void BowlingLane::resetPins() {
    if (mPinsInitialized) {
        for (auto& pin : mPins) {
            if (pin) {
                pin->reset();
            }
        }
    }
}

int BowlingLane::countKnockedDownPins() const {
    int knockedDownCount = 0;
    
    if (mPinsInitialized) {
        for (const auto& pin : mPins) {
            if (pin && pin->isKnockedDown()) {
                knockedDownCount++;
            }
        }
    }
    
    return knockedDownCount;
}