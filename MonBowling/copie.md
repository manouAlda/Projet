#include "../../include/objects/BowlingLane.h"

BowlingLane::BowlingLane(Ogre::SceneManager* sceneMgr)
    : mSceneMgr(sceneMgr),
      mLaneNode(nullptr),
      mLaneEntity(nullptr),
      mGutterLeftNode(nullptr),
      mGutterLeftEntity(nullptr),
      mGutterRightNode(nullptr),
      mGutterRightEntity(nullptr),
      mPinDeckNode(nullptr),
      mPinDeckEntity(nullptr),
      mPinsInitialized(false)
{
    // Initialisation des quilles avec une taille standard de 10
    mPins.resize(10);
}

BowlingLane::~BowlingLane() {}

void BowlingLane::create(const Ogre::Vector3& ballStartPosition) {
    // On sauvegarde la position de départ de la boule pour positionner correctement la piste
    mBallStartPosition = ballStartPosition;
    
    // Création de la piste
    createLane();
    
    // Création des gouttières
    //createGutters();
    
    // Création de la zone des quilles
    //createPinDeck();
    
    // Création et positionnement des quilles
    setupPins();
}

void BowlingLane::createLane() {
    // Création du nœud pour la piste
    mLaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BowlingLaneNode");
    
    mLaneEntity = mSceneMgr->createEntity("LaneEntity", "lane.mesh");
    mLaneNode->attachObject(mLaneEntity);
    
    // Positionnement de la piste
    float laneZPosition = mBallStartPosition.z + 150.0f;
    mLaneNode->setPosition(mBallStartPosition.x, 20.0f, laneZPosition); // Restauré à y = 20.0f
    
    // Ajout d'une mise à l'échelle pour agrandir le modèle
    mLaneNode->setScale(10.0f, 10.0f, 10.0f);
    
    // Ajout de la piste au monde physique comme objet statique
    btRigidBody* laneBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, mLaneEntity, Ogre::Bullet::CT_TRIMESH);
    
    if (laneBody) {
        laneBody->setFriction(0.3f);
    }

    Ogre::LogManager::getSingleton().logMessage("Piste à : " + Ogre::StringConverter::toString(mLaneNode->getPosition()));
}

void BowlingLane::createGutters() {
    // Création des gouttières à gauche et à droite de la piste
    float gutterWidth = 10.0f;
    float gutterDepth = 5.0f;
    float laneWidth = 60.0f;
    float laneLength = 300.0f;
    
    // Gouttière gauche
    mGutterLeftNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GutterLeftNode");
    Ogre::Entity* gutterLeftEntity = mSceneMgr->createEntity("GutterLeftEntity", "cube.mesh");
    
    // Création d'un matériau pour les gouttières
    Ogre::MaterialPtr gutterMaterial = Ogre::MaterialManager::getSingleton().create(
        "GutterMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Technique* technique = gutterMaterial->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    
    // Couleur grise pour les gouttières
    pass->setDiffuse(Ogre::ColourValue(0.5f, 0.5f, 0.5f, 1.0f));
    
    gutterLeftEntity->setMaterialName("GutterMaterial");
    mGutterLeftNode->attachObject(gutterLeftEntity);
    
    // Mise à l'échelle et positionnement
    mGutterLeftNode->setScale(gutterWidth, gutterDepth, laneLength);
    mGutterLeftNode->setPosition(
        mBallStartPosition.x - laneWidth/2 - gutterWidth/2, 
        -gutterDepth/2, 
        mLaneNode->getPosition().z);
    
    // Gouttière droite
    mGutterRightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GutterRightNode");
    Ogre::Entity* gutterRightEntity = mSceneMgr->createEntity("GutterRightEntity", "cube.mesh");
    gutterRightEntity->setMaterialName("GutterMaterial");
    mGutterRightNode->attachObject(gutterRightEntity);
    
    mGutterRightNode->setScale(gutterWidth, gutterDepth, laneLength);
    mGutterRightNode->setPosition(
        mBallStartPosition.x + laneWidth/2 + gutterWidth/2, 
        -gutterDepth/2, 
        mLaneNode->getPosition().z);
    
    // Ajout des gouttières au monde physique
    btRigidBody* gutterLeftBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, gutterLeftEntity, Ogre::Bullet::CT_BOX);
    btRigidBody* gutterRightBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, gutterRightEntity, Ogre::Bullet::CT_BOX);
    
    if (gutterLeftBody && gutterRightBody) {
        gutterLeftBody->setFriction(0.1f);  // Faible friction pour les gouttières
        gutterRightBody->setFriction(0.1f);
    }
}

void BowlingLane::createPinDeck() {
    // Création de la zone des quilles (au bout de la piste)
    mPinDeckNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PinDeckNode");
    
    // La zone des quilles est un plan légèrement surélevé
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 20.0f); // Ajusté au sol à y = 20.0f
    Ogre::MeshManager::getSingleton().createPlane(
        "pin_deck_mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 60.0f, 30.0f, 1, 1, true, 1, 5.0f, 5.0f, Ogre::Vector3::UNIT_Z);
    
    mPinDeckEntity = mSceneMgr->createEntity("PinDeckEntity", "pin_deck_mesh");
    
    // Création d'un matériau pour la zone des quilles
    Ogre::MaterialPtr pinDeckMaterial = Ogre::MaterialManager::getSingleton().create(
        "PinDeckMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Technique* technique = pinDeckMaterial->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    
    // Couleur légèrement différente pour la zone des quilles
    pass->setDiffuse(Ogre::ColourValue(0.85f, 0.65f, 0.45f, 1.0f));
    
    mPinDeckEntity->setMaterialName("PinDeckMaterial");
    mPinDeckNode->attachObject(mPinDeckEntity);
    
    // Positionnement à la fin de la piste
    float pinDeckZPosition = mLaneNode->getPosition().z - 135.0f; // Ajustez selon la longueur de votre piste
    mPinDeckNode->setPosition(mBallStartPosition.x, 21.0f, pinDeckZPosition); // 1 unité au-dessus du sol
    
    // Ajout de la zone des quilles au monde physique
    btRigidBody* pinDeckBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, mPinDeckEntity, Ogre::Bullet::CT_BOX);
    
    if (pinDeckBody) {
        pinDeckBody->setFriction(0.5f); // Friction modérée pour la zone des quilles
    }
}

void BowlingLane::setupPins() {
    // Calculer la position des quilles directement par rapport à mBallStartPosition
    float pinZOffset = -80.0f; // 80 unités derrière la boule (z négatif)
    Ogre::Vector3 pinBasePosition = Ogre::Vector3(
        mBallStartPosition.x,     // Même axe X que la boule
        21.0f,                   // 1 unité au-dessus du sol (y = 20.0f)
        mBallStartPosition.z + pinZOffset  // Derrière la boule
    );
    
    // Espacement entre les quilles
    float spacing = 2.0f;
    
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
    // Mise à jour de toutes les quilles
    if (mPinsInitialized) {
        for (auto& pin : mPins) {
            if (pin) {
                pin->update(deltaTime);
            }
        }
    }
}

void BowlingLane::resetPins() {
    // Réinitialisation de toutes les quilles à leur position d'origine
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
********************************************************************************
#include "../../include/managers/CameraController.h"
#include <OgreStringConverter.h>
#include <OgreSceneQuery.h>
#include <OgreViewport.h>  // Ajout pour résoudre l'erreur avec Viewport
#include <OgreLogManager.h> // Ajout pour résoudre l'erreur avec LogManager

CameraController::CameraController(Ogre::Camera* camera, Ogre::SceneManager* sceneMgr)
    : mCamera(camera), mSceneMgr(sceneMgr), mCameraMoving(false), mRotating(false), mYaw(0), mPitch(0) {
    mCameraNode = camera->getParentSceneNode();
}

void CameraController::handleMouseClick(const OgreBites::MouseButtonEvent& evt) {
    if (evt.button == OgreBites::BUTTON_LEFT) {
        // Créer un rayon à partir du clic
        Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
            evt.x / (float)mCamera->getViewport()->getActualWidth(),
            evt.y / (float)mCamera->getViewport()->getActualHeight());

        // Log pour déboguer les coordonnées du clic
        Ogre::LogManager::getSingleton().logMessage("Clic écran à : x=" + Ogre::StringConverter::toString(evt.x) + 
                                                    ", y=" + Ogre::StringConverter::toString(evt.y));

        // Configurer le RaySceneQuery
        Ogre::RaySceneQuery* raySceneQuery = mSceneMgr->createRayQuery(mouseRay);
        raySceneQuery->setSortByDistance(true);
        raySceneQuery->setQueryMask(Ogre::SceneManager::ENTITY_TYPE_MASK);

        // Exécuter le raycast
        Ogre::RaySceneQueryResult& result = raySceneQuery->execute();
        Ogre::Vector3 targetPoint;
        bool hitFound = false;

        for (const auto& entry : result) {
            if (entry.movable && entry.distance > 0.0f) {
                targetPoint = mouseRay.getPoint(entry.distance);
                Ogre::LogManager::getSingleton().logMessage("Objet touché : " + entry.movable->getName() + 
                                                            " à distance : " + Ogre::StringConverter::toString(entry.distance) +
                                                            ", point : " + Ogre::StringConverter::toString(targetPoint));
                hitFound = true;
                break;
            }
        }

        if (!hitFound) {
            // Intersection avec le plan de la piste (y=20.0f)
            Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 20.0f);
            std::pair<bool, Ogre::Real> intersect = mouseRay.intersects(groundPlane);
            if (intersect.first) {
                targetPoint = mouseRay.getPoint(intersect.second);
                Ogre::LogManager::getSingleton().logMessage("Sol touché à : " + Ogre::StringConverter::toString(targetPoint));
                hitFound = true;
            }
        }

        if (hitFound) {
            // Limites de distance
            const float minDistance = 20.0f;
            const float maxDistance = 100.0f;

            // Calculer la nouvelle position de la caméra
            Ogre::Vector3 currentPos = mCameraNode->getPosition();
            float distance = (currentPos - targetPoint).length();
            float clampedDistance = std::max(minDistance, std::min(maxDistance, distance));
            Ogre::Vector3 direction = (targetPoint - currentPos).normalisedCopy();
            Ogre::Vector3 newPos = targetPoint - direction * clampedDistance;
            newPos.y = 35.0f + 20.0f; // Hauteur fixe ajustée à 55.0f (35 au-dessus du sol à y=20)

            // Log pour déboguer le calcul
            Ogre::LogManager::getSingleton().logMessage("Position actuelle : " + Ogre::StringConverter::toString(currentPos));
            Ogre::LogManager::getSingleton().logMessage("Direction : " + Ogre::StringConverter::toString(direction));
            Ogre::LogManager::getSingleton().logMessage("Distance initiale : " + Ogre::StringConverter::toString(distance));

            // Limites de la zone (ajusté pour inclure les quilles)
            const float minX = -212.553f; // -210.053 - 2.5
            const float maxX = -207.553f; // -210.053 + 2.5
            const float minZ = -222.784f; // -212.784 - 10
            const float maxZ = -122.784f; // -132.784 + 10
            newPos.x = std::max(minX, std::min(maxX, newPos.x));
            newPos.z = std::max(minZ, std::min(maxZ, newPos.z));

            // Réinitialiser les angles de rotation
            mYaw = 0;
            mPitch = 0;

            // Définir les cibles
            mCameraTargetPos = newPos;
            mCameraTargetLookAt = targetPoint;
            mCameraMoving = true;

            Ogre::LogManager::getSingleton().logMessage("Point ciblé à : " + Ogre::StringConverter::toString(targetPoint));
            Ogre::LogManager::getSingleton().logMessage("Nouvelle position cible : " + Ogre::StringConverter::toString(newPos) +
                                                        ", distance : " + Ogre::StringConverter::toString(clampedDistance));
            // Log explicite du lookAt après un clic
            Ogre::LogManager::getSingleton().logMessage("LookAt après clic : " + Ogre::StringConverter::toString(mCameraTargetLookAt));
        } else {
            Ogre::LogManager::getSingleton().logMessage("Aucun point touché.");
        }

        // Nettoyer
        mSceneMgr->destroyQuery(raySceneQuery);
    }
}

void CameraController::handleMousePressed(const OgreBites::MouseButtonEvent& evt) {
    if (evt.button == OgreBites::BUTTON_RIGHT) {
        mRotating = true;
        mLastMousePos = Ogre::Vector2(evt.x, evt.y);
        Ogre::LogManager::getSingleton().logMessage("Clic droit : Début rotation à x=" + 
                                                    Ogre::StringConverter::toString(evt.x) + ", y=" + 
                                                    Ogre::StringConverter::toString(evt.y));
    }
}

void CameraController::handleMouseMoved(const OgreBites::MouseMotionEvent& evt) {
    if (mRotating) {
        // Calculer le déplacement de la souris
        Ogre::Vector2 currentMousePos(evt.x, evt.y);
        Ogre::Vector2 mouseDelta = currentMousePos - mLastMousePos;

        // Mettre à jour les angles de rotation (sensibilité augmentée)
        mYaw -= Ogre::Radian(Ogre::Degree(mouseDelta.x * 1.5f)); // 1.5 degré par pixel
        mPitch -= Ogre::Radian(Ogre::Degree(mouseDelta.y * 1.5f));

        // Limiter le pitch (±30 degrés)
        const Ogre::Radian maxPitch = Ogre::Degree(30);
        mPitch = std::max(-maxPitch, std::min(maxPitch, mPitch));

        // Mettre à jour la dernière position
        mLastMousePos = currentMousePos;

        // Calculer la nouvelle position de la caméra
        Ogre::Vector3 lookAtPos = mCameraTargetLookAt;
        if (lookAtPos == Ogre::Vector3::ZERO) {
            lookAtPos = Ogre::Vector3(-210.053, 20.0, -132.784); // Ajusté à y = 20.0
        }

        Ogre::Vector3 currentPos = mCameraNode->getPosition();
        float distance = (currentPos - lookAtPos).length();
        const float minDistance = 20.0f;
        const float maxDistance = 100.0f;
        float clampedDistance = std::max(minDistance, std::min(maxDistance, distance));

        // Calculer la nouvelle position avec yaw et pitch
        Ogre::Quaternion yawQuat(Ogre::Radian(mYaw), Ogre::Vector3::UNIT_Y);
        Ogre::Quaternion pitchQuat(Ogre::Radian(mPitch), Ogre::Vector3::UNIT_X);
        Ogre::Quaternion rot = yawQuat * pitchQuat;
        Ogre::Vector3 direction = rot * Ogre::Vector3(0, 0, -1); // Direction vers l'arrière
        Ogre::Vector3 newPos = lookAtPos + direction * clampedDistance;

        // Conserver la hauteur et limiter la zone (ajusté pour inclure les quilles)
        newPos.y = 35.0f + 20.0f; // Hauteur fixe ajustée à 55.0f
        const float minX = -212.553f; // -210.053 - 2.5
        const float maxX = -207.553f; // -210.053 + 2.5
        const float minZ = -222.784f; // -212.784 - 10
        const float maxZ = -122.784f; // -132.784 + 10
        newPos.x = std::max(minX, std::min(maxX, newPos.x));
        newPos.z = std::max(minZ, std::min(maxZ, newPos.z));

        // Mettre à jour les cibles
        mCameraTargetPos = newPos;
        mCameraMoving = true;

        Ogre::LogManager::getSingleton().logMessage("Rotation : yaw=" + Ogre::StringConverter::toString(mYaw.valueDegrees()) + 
                                                    ", pitch=" + Ogre::StringConverter::toString(mPitch.valueDegrees()) +
                                                    ", nouvelle position : " + Ogre::StringConverter::toString(newPos) +
                                                    ", distance : " + Ogre::StringConverter::toString(clampedDistance));
    }
}

void CameraController::update(const Ogre::FrameEvent& evt) {
    if (!mCameraMoving) {
        if (mRotating) {
            // Arrêter la rotation si le bouton droit est relâché
            mRotating = false;
        }
        // Log de la position et du lookAt de la caméra même si elle ne bouge pas
        Ogre::Vector3 camPos = mCameraNode->getPosition();
        Ogre::LogManager::getSingleton().logMessage("Position de la caméra : " + Ogre::StringConverter::toString(camPos));
        Ogre::LogManager::getSingleton().logMessage("LookAt de la caméra : " + Ogre::StringConverter::toString(mCameraTargetLookAt));
        return;
    }

    // Interpolation fluide de la position
    const float tightness = 15.0f * evt.timeSinceLastFrame;
    Ogre::Vector3 currentPos = mCameraNode->getPosition();
    Ogre::Vector3 newPos = currentPos + (mCameraTargetPos - currentPos) * tightness;
    mCameraNode->setPosition(newPos);

    // Orientation vers le point cible
    mCameraNode->lookAt(mCameraTargetLookAt, Ogre::Node::TS_WORLD);

    // Appliquer le pitch limité
    Ogre::Vector3 xAxis = mCameraNode->getOrientation().xAxis();
    Ogre::Vector3 zAxis = mCameraNode->getOrientation().zAxis();
    Ogre::Quaternion yawQuat = mCameraNode->getOrientation();
    if (mPitch != Ogre::Radian(0)) {
        Ogre::Quaternion pitchQuat(mPitch, xAxis);
        mCameraNode->setOrientation(yawQuat * pitchQuat);
    } else {
        mCameraNode->setOrientation(Ogre::Quaternion(xAxis, Ogre::Vector3::UNIT_Y, zAxis));
    }

    // Log pour déboguer
    Ogre::LogManager::getSingleton().logMessage("Caméra à : " + Ogre::StringConverter::toString(newPos) + 
                                                ", regarde : " + Ogre::StringConverter::toString(mCameraTargetLookAt));

    // Arrêter le mouvement si proche de la cible
    if ((newPos - mCameraTargetPos).length() < 0.01f) {
        mCameraMoving = false;
    }
}