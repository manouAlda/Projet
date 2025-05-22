Je veux que tu m'aides à créer un jeu de bowling complet avec OGRE3D et Bullet Physics. Voici les étapes que je souhaite suivre :

1. CRÉATION DES ÉLÉMENTS FONDAMENTAUX DU BOWLING

1.1 Modélisation de la piste de bowling
- Crée une piste de bowling réaliste avec gouttières
- Utilise les dimensions standard (longueur 18m, largeur 1.05m)
- Ajoute des gouttières de chaque côté avec la bonne profondeur
- Implémente des murs latéraux pour contenir la boule
- Applique des textures et matériaux appropriés pour l'apparence
- Configure les propriétés physiques (friction, rebond) via Bullet

1.2 Création de la boule de bowling
- Modélise une boule de bowling avec la taille et le poids réalistes
- Implémente les propriétés physiques correctes (masse, friction, rebond)
- Ajoute la possibilité de contrôler la rotation et l'effet de la boule
- Permets différentes couleurs/textures pour personnalisation
- Assure une interaction réaliste avec la piste et les gouttières

1.3 Implémentation des quilles
- Crée un modèle de quille avec les dimensions standard
- Place les 10 quilles dans la formation triangulaire correcte
- Configure les propriétés physiques pour des collisions réalistes
- Implémente la détection de quilles tombées (angle > 30°)
- Ajoute un système de réinitialisation des quilles entre les lancers

2. MÉCANIQUE DE JEU

2.1 Système de visée et lancer
- Implémente un système de visée avec indicateur visuel
- Crée un contrôle de puissance (barre ou jauge)
- Ajoute un système pour appliquer un effet à la boule
- Développe une caméra qui suit la boule pendant le lancer
- Permets l'annulation et la réinitialisation du lancer

2.2 Détection des quilles et calcul du score
- Détecte quelles quilles sont tombées après chaque lancer
- Implémente un système pour compter les quilles renversées
- Crée une logique pour déterminer les strikes et spares
- Développe un système pour suivre les lancers et les frames
- Assure la détection correcte des quilles qui tombent en cascade

3. SYSTÈME DE SCORE

3.1 Règles officielles du bowling
- Implémente les 10 frames standard d'une partie
- Gère correctement les strikes (10 + 2 lancers suivants)
- Gère correctement les spares (10 + 1 lancer suivant)
- Ajoute la logique pour les lancers bonus dans la 10ème frame
- Calcule le score cumulatif selon les règles officielles

3.2 Interface du tableau de score
- Crée un affichage visuel du tableau de score
- Montre clairement les strikes, spares et lancers normaux
- Affiche le score cumulatif pour chaque frame
- Indique le joueur actif et le lancer en cours
- Ajoute des animations pour les strikes et spares

3.3 Gestion des joueurs
- Permets l'ajout de plusieurs joueurs
- Implémente un système de tours entre les joueurs
- Sauvegarde et charge les scores des joueurs
- Ajoute un système de classement
- Permets la personnalisation des noms des joueurs

4. AMÉLIORATIONS PHYSIQUES ET VISUELLES

4.1 Interactions physiques avancées
- Affine les propriétés de friction de la piste pour un comportement réaliste
- Optimise les collisions entre la boule et les quilles
- Implémente des effets de rotation réalistes pour la boule
- Ajoute des variations subtiles dans les propriétés physiques des quilles
- Crée des effets de rebond réalistes contre les murs et gouttières

4.2 Éléments audiovisuels
- Ajoute des sons pour les collisions, strikes, spares
- Implémente une ambiance sonore de bowling
- Crée des effets visuels pour les strikes et spares
- Ajoute des animations pour les quilles qui tombent
- Implémente des effets de particules pour les impacts importants

Pour chaque élément, je veux que tu m'expliques comment modéliser ou charger les mesh, gérer le placement, configurer l'apparence et définir les propriétés physiques appropriées. Utilise OGRE3D pour le rendu et Bullet Physics pour la simulation physique, en suivant les patterns de conception appropriés (Singleton, Factory, Observer, State).

**********************************************************
1- une flêche directionnelle fera l'affaire. une barre qui se remplit automatiquement (à arrêter au bon moment). 3- Pour appliquer un effet à la boule, un curseur latéral fera cela. 4- Une transistion fluide plus tôt qu'un changement instantané.Oui, après le succés ou failure du lancé, revenir à la position initiale. 5- Pour annuler un lancé, un boutton spécifique. Concernant la détection des quilles: 1- Juste en haut à droite de couleur blanc. 2- Non, d'abord faire fonctionner le jeu sans règle. 3- attendre un délai minimum après le premier impact (par exemple 1-2 secondes),Une quille est considérée comme tombée si son inclinaison dépasse un certain seuil (généralement 45° par rapport à la verticale). 4- Voir par rapport au jeu.
***********************************************************************


https://manus.im/share/mSXScx6R5YQ1f45BpnXNnj?replay=1

// Modélisation d'une quille de bowling simple - pin
module bowling_pin() {
    difference() {
        // Corps principal
        scale([1,1,2.5])
            sphere(r=10);
        // Trou creux dessous
        translate([0,0,-5])
            cylinder(h=15, r=6);
    }
}

// Placement des 10 quilles selon la formation standard:
// Rangée 1 : 1 quille
// Rangée 2 : 2 quilles
// Rangée 3 : 3 quilles
// Rangée 4 : 4 quilles

// Distance entre quilles dans une rangée et entre rangées
pin_spacing = 25;
rows = [1, 2, 3, 4];

// Fonction pour placer une rangée de pins centrée
module pin_row(count, row_index) {
    offset = -(count - 1) * pin_spacing / 2;
    for(i = [0 : count - 1]) {
        translate([offset + i * pin_spacing, 0, 0]) 
            bowling_pin();
    }
}

module bowling_pins() {
    for(row = [0 : 3]) {
        translate([0, -row*pin_spacing, 0])
            pin_row(rows[row], row);
    }
}

bowling_pins();


// Dans BowlingLane::create(), modifiez la partie des gouttières :

// Gouttière gauche
Ogre::Plane leftGutterPlane(Ogre::Vector3::UNIT_Y, 0);
Ogre::MeshManager::getSingleton().createPlane(
    "leftGutter", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    leftGutterPlane, mLength, mGutterWidth, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

mLeftGutterEntity = mSceneMgr->createEntity("LeftGutterEntity", "leftGutter");
mLeftGutterEntity->setMaterialName("GutterMaterial");

Ogre::SceneNode* leftGutterNode = mLaneNode->createChildSceneNode("LeftGutterNode");
leftGutterNode->attachObject(mLeftGutterEntity);
leftGutterNode->setPosition(-(mWidth/2 + mGutterWidth/2), -mGutterDepth/2, 0);
// Rotation pour simuler une gouttière inclinée
leftGutterNode->roll(Ogre::Degree(15));

// Gouttière droite (même principe avec rotation inverse)
// ...
rightGutterNode->roll(Ogre::Degree(-15));

****************************
void BowlingBall::setColor(const Ogre::ColourValue& color) {
    mColor = color;

    // Ensure the material exists
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("BowlingBallMaterial");
    if (!material) {
        Ogre::LogManager::getSingleton().logError("BowlingBall::setColor - Material 'BowlingBallMaterial' does not exist.");
        return;
    }

    // Update the material's diffuse color
    Ogre::Technique* technique = material->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    pass->setDiffuse(mColor);
}
************************************
#include "../../include/managers/CameraController.h"
#include <OgreStringConverter.h>
#include <OgreSceneQuery.h>

CameraController::CameraController(Ogre::Camera* camera, Ogre::SceneManager* sceneMgr)
    : mCamera(camera), mSceneMgr(sceneMgr), mCameraMoving(false) {
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

        // Exécuter le raycast
        Ogre::RaySceneQueryResult& result = raySceneQuery->execute();
        Ogre::Vector3 targetPoint;
        bool hitFound = false;

        for (const auto& entry : result) {
            if (entry.movable) {
                // Calculer le point exact d'intersection avec l'objet
                targetPoint = mouseRay.getPoint(entry.distance);
                Ogre::LogManager::getSingleton().logMessage("Objet touché : " + entry.movable->getName() + 
                                                            " à distance : " + Ogre::StringConverter::toString(entry.distance) +
                                                            ", point : " + Ogre::StringConverter::toString(targetPoint));
                hitFound = true;
                break;
            }
        }

        if (!hitFound) {
            // Si aucun objet n'est touché, calculer l'intersection avec le plan du sol
            // Ajuste la hauteur du sol selon ta piste (par exemple, y = 0 ou y = 10)
            Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0); // Change 0 si le sol n'est pas à y=0
            std::pair<bool, Ogre::Real> intersect = mouseRay.intersects(groundPlane);
            if (intersect.first) {
                // Point d'intersection avec le sol
                targetPoint = mouseRay.getPoint(intersect.second);
                Ogre::LogManager::getSingleton().logMessage("Sol touché à : " + Ogre::StringConverter::toString(targetPoint));
                hitFound = true;
            }
        }

        if (hitFound) {
            // Calculer la nouvelle position de la caméra (même hauteur)
            Ogre::Vector3 currentPos = mCameraNode->getPosition();
            float distance = (currentPos - targetPoint).length();
            Ogre::Vector3 direction = (targetPoint - currentPos).normalisedCopy();
            Ogre::Vector3 newPos = targetPoint - direction * distance;
            newPos.y = currentPos.y; // Vue horizontale

            // Définir les cibles pour l'animation
            mCameraTargetPos = newPos;
            mCameraTargetLookAt = targetPoint;
            mCameraMoving = true;

            Ogre::LogManager::getSingleton().logMessage("Point ciblé à : " + Ogre::StringConverter::toString(targetPoint));
        } else {
            Ogre::LogManager::getSingleton().logMessage("Aucun point touché.");
        }

        // Nettoyer
        mSceneMgr->destroyQuery(raySceneQuery);
    }
}

void CameraController::update(const Ogre::FrameEvent& evt) {
    if (!mCameraMoving) return;

    // Interpolation fluide de la position
    const float tightness = 15.0f * evt.timeSinceLastFrame; // Vitesse encore plus rapide
    Ogre::Vector3 currentPos = mCameraNode->getPosition();
    Ogre::Vector3 newPos = currentPos + (mCameraTargetPos - currentPos) * tightness;
    mCameraNode->setPosition(newPos);

    // Interpolation de l'orientation
    mCameraNode->lookAt(mCameraTargetLookAt, Ogre::Node::TS_WORLD);

    // Supprimer le pitch pour une vue horizontale
    Ogre::Vector3 xAxis = mCameraNode->getOrientation().xAxis();
    Ogre::Vector3 zAxis = mCameraNode->getOrientation().zAxis();
    mCameraNode->setOrientation(Ogre::Quaternion(xAxis, Ogre::Vector3::UNIT_Y, zAxis));

    // Log pour déboguer la position finale de la caméra
    Ogre::LogManager::getSingleton().logMessage("Caméra à : " + Ogre::StringConverter::toString(newPos) + 
                                                ", regarde : " + Ogre::StringConverter::toString(mCameraTargetLookAt));

    // Arrêter le mouvement si proche de la cible
    if ((newPos - mCameraTargetPos).length() < 0.1f) {
        mCameraMoving = false;
    }
}

*******************************************************************************************
Clic écran à : x=294, y=497
Objet touché : GroundEntity à distance : 160.711, point : -210.053 0 -132.784
Point ciblé à : -210.053 0 -132.784
Nouvelle position cible : -250 35 20
Caméra à : -250 35 20, regarde : -210.053 0 -132.784
********************************************************************************************
#include "../../include/managers/CameraController.h"
#include <OgreStringConverter.h>
#include <OgreSceneQuery.h>

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
            // Intersection avec le plan de la piste (y=20)
            Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 20);
            std::pair<bool, Ogre::Real> intersect = mouseRay.intersects(groundPlane);
            if (intersect.first) {
                targetPoint = mouseRay.getPoint(intersect.second);
                Ogre::LogManager::getSingleton().logMessage("Sol touché à : " + Ogre::StringConverter::toString(targetPoint));
                hitFound = true;
            }
        }

        if (hitFound) {
            // Calculer la nouvelle position de la caméra (même distance)
            Ogre::Vector3 currentPos = mCameraNode->getPosition();
            float distance = (currentPos - targetPoint).length();
            Ogre::Vector3 direction = (targetPoint - currentPos).normalisedCopy();
            Ogre::Vector3 newPos = targetPoint - direction * distance;
            newPos.y = 35.0f; // Hauteur fixe

            // Réinitialiser les angles de rotation
            mYaw = 0;
            mPitch = 0;

            // Définir les cibles
            mCameraTargetPos = newPos;
            mCameraTargetLookAt = targetPoint;
            mCameraMoving = true;

            Ogre::LogManager::getSingleton().logMessage("Point ciblé à : " + Ogre::StringConverter::toString(targetPoint));
            Ogre::LogManager::getSingleton().logMessage("Nouvelle position cible : " + Ogre::StringConverter::toString(newPos));
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

        // Mettre à jour les angles de rotation (sensibilité ajustée)
        mYaw -= Ogre::Radian(Ogre::Degree(mouseDelta.x * 1.5f));
        mPitch -= Ogre::Radian(Ogre::Degree(mouseDelta.y * 1.5f));

        // Limiter le pitch (±30 degrés)
        const Ogre::Radian maxPitch = Ogre::Degree(30);
        mPitch = std::max(-maxPitch, std::min(maxPitch, mPitch));

        // Mettre à jour la dernière position
        mLastMousePos = currentMousePos;

        // Calculer la nouvelle position de la caméra
        Ogre::Vector3 lookAtPos = mCameraTargetLookAt;
        if (lookAtPos == Ogre::Vector3::ZERO) {
            lookAtPos = Ogre::Vector3(-160, 20, -150); // Point central par défaut
        }

        Ogre::Vector3 currentPos = mCameraNode->getPosition();
        float distance = (currentPos - lookAtPos).length();

        // Calculer la nouvelle position avec yaw et pitch
        Ogre::Quaternion yawQuat(Ogre::Radian(mYaw), Ogre::Vector3::UNIT_Y);
        Ogre::Quaternion pitchQuat(Ogre::Radian(mPitch), Ogre::Vector3::UNIT_X);
        Ogre::Quaternion rot = yawQuat * pitchQuat;
        Ogre::Vector3 direction = rot * Ogre::Vector3(0, 0, -1); // Direction vers l'arrière
        Ogre::Vector3 newPos = lookAtPos + direction * distance;

        // Conserver la hauteur actuelle
        newPos.y = 35.0f;

        // Mettre à jour les cibles
        mCameraTargetPos = newPos;
        mCameraMoving = true;

        Ogre::LogManager::getSingleton().logMessage("Rotation : yaw=" + Ogre::StringConverter::toString(mYaw.valueDegrees()) + 
                                                    ", pitch=" + Ogre::StringConverter::toString(mPitch.valueDegrees()) +
                                                    ", nouvelle position : " + Ogre::StringConverter::toString(newPos));
    }
}

void CameraController::update(const Ogre::FrameEvent& evt) {
    if (!mCameraMoving) {
        if (mRotating) {
            // Arrêter la rotation si le bouton droit est relâché
            mRotating = false;
        }
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
********************************************* bowlingLAne*********************
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

BowlingLane::~BowlingLane() {
    // Le nettoyage des objets scène est géré par Ogre
}

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
    mLaneNode->setPosition(mBallStartPosition.x, 20.0f, laneZPosition);
    
    // Ajout d'une mise à l'échelle pour agrandir le modèle
    mLaneNode->setScale(10.0f, 10.0f, 10.0f);
    
    // Ajout de la piste au monde physique comme objet statique
    btRigidBody* laneBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, mLaneEntity, Ogre::Bullet::CT_TRIMESH);
    
    if (laneBody) {
        laneBody->setFriction(0.3f);
    }
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
/*    
    // La zone des quilles est un plan légèrement surélevé
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
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
*/    
    // Positionnement à la fin de la piste
    float pinDeckZPosition = mLaneNode->getPosition().z - 135.0f; // Ajustez selon la longueur de votre piste
    mPinDeckNode->setPosition(mBallStartPosition.x, 0.1f, pinDeckZPosition); // Légèrement surélevé (0.1)
    
    // Ajout de la zone des quilles au monde physique
/*    
    btRigidBody* pinDeckBody = PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(
        0.0f, mPinDeckEntity, Ogre::Bullet::CT_BOX);
    
    if (pinDeckBody) {
        pinDeckBody->setFriction(0.5f); // Friction modérée pour la zone des quilles
    }
*/
}

void BowlingLane::setupPins() {

    // Calculate position based on lane position instead
    float pinZPosition = mLaneNode->getPosition().z - 135.0f;
    
    Ogre::Vector3 pinBasePosition = Ogre::Vector3(
        mBallStartPosition.x,     // Même axe X que la boule
        1.0f,                    // Légèrement au-dessus du sol
        pinZPosition - 10.0f     // Au niveau de la zone des quilles, calculé directement
    );
    
    // Espacement entre les quilles
    float spacing = 2.0f;
    
    // Position des quilles en formation triangulaire standard
    // Rangée 1 (la plus éloignée, une seule quille)
    Ogre::Vector3 pinPositions[10];
    pinPositions[0] = pinBasePosition;
    
    // Rangée 2 (deux quilles)
    pinPositions[1] = pinBasePosition + Ogre::Vector3(-spacing, 0, spacing);
    pinPositions[2] = pinBasePosition + Ogre::Vector3(spacing, 0, spacing);
    
    // Rangée 3 (trois quilles)
    pinPositions[3] = pinBasePosition + Ogre::Vector3(-2*spacing, 0, 2*spacing);
    pinPositions[4] = pinBasePosition + Ogre::Vector3(0, 0, 2*spacing);
    pinPositions[5] = pinBasePosition + Ogre::Vector3(2*spacing, 0, 2*spacing);
    
    // Rangée 4 (quatre quilles, la plus proche)
    pinPositions[6] = pinBasePosition + Ogre::Vector3(-3*spacing, 0, 3*spacing);
    pinPositions[7] = pinBasePosition + Ogre::Vector3(-spacing, 0, 3*spacing);
    pinPositions[8] = pinBasePosition + Ogre::Vector3(spacing, 0, 3*spacing);
    pinPositions[9] = pinBasePosition + Ogre::Vector3(3*spacing, 0, 3*spacing);
    
    // Création des quilles
    for (int i = 0; i < 10; ++i) {
        mPins[i] = std::make_unique<BowlingPin>(mSceneMgr);
        mPins[i]->create(pinPositions[i], i+1);
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
*****************************************************************************************************
/*
void Application::setup(){
    // Configuration de base
    OgreBites::ApplicationContext::setup();
    addInputListener(this);
    
    // Création du SceneManager
    Ogre::Root* root = getRoot();
    mSceneMgr = root->createSceneManager();
    
    // Configuration du shader system
    Ogre::RTShader::ShaderGenerator* shadergen = 
        Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(mSceneMgr);
    
    // Création de la caméra
    mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mCamera = mSceneMgr->createCamera("MainCamera");
    mCameraNode->attachObject(mCamera);
    // Positionnement de la caméra pour voir la scène
    mCameraNode->setPosition(Ogre::Vector3(0, 15, 30));  // Plus haute et plus éloignée
    mCameraNode->lookAt(Ogre::Vector3(0, 5, 0), Ogre::Node::TS_WORLD);  // Regarder vers le centre de la scène
    
    mCamera->setNearClipDistance(1);  // Réduire la distance de clipping proche
    
    // Configuration du viewport
    Ogre::Viewport* vp = getRenderWindow()->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
    
    // Création de la scène
    createScene();
    
    // Configuration de la physique
    setupPhysics();
}
*/
**********************************************************************************************************
/*
void Application::createScene()
{
    // Lumière ambiante plus forte
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));
    
    // Création d'une lumière directionnelle avec SceneNode (méthode moderne)
    Ogre::SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    lightNode->attachObject(light);
    lightNode->setDirection(Ogre::Vector3(-0.5, -0.5, -0.5));
    
    // Ajout d'une deuxième lumière pour mieux éclairer la scène
    Ogre::SceneNode* lightNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light2 = mSceneMgr->createLight("SecondLight");
    light2->setType(Ogre::Light::LT_DIRECTIONAL);
    lightNode2->attachObject(light2);
    lightNode2->setDirection(Ogre::Vector3(0.5, -0.5, 0.5));
}
*/

********************************************************************************************
/*
btRigidBody* ObjectFactory::createStaticGround(){
    // Création du plan dans Ogre
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -0.1);  // Positionné légèrement plus bas
    Ogre::MeshManager::getSingleton().createPlane(
        "ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 3000, 3000, 10, 10, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
    
    // Création de l'entité et du nœud
    Ogre::Entity* groundEntity = mSceneMgr->createEntity("GroundEntity", "ground");
    
    // Création d'un matériau simple pour le sol avec une couleur plus neutre
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        "GroundMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Technique* technique = material->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    pass->setDiffuse(Ogre::ColourValue(0.3, 0.3, 0.3));  // Gris foncé
    pass->setSpecular(Ogre::ColourValue(0.1, 0.1, 0.1));
    pass->setShininess(30);
    
    groundEntity->setMaterialName("GroundMaterial");
    
    Ogre::SceneNode* groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode");
    groundNode->attachObject(groundEntity);
    
    // Ajout du sol au monde physique (masse 0 = objet statique)
    return PhysicsManager::getInstance()->getDynamicsWorld()->addRigidBody(0.0f, groundEntity, Ogre::Bullet::CT_BOX);
}

*/
***********************************************************************
#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>

#include "../objects/ObjectFactory.h"
#include "../objects/BowlingLane.h"
#include "../objects/BowlingBall.h"
#include "../managers/CameraController.h"

class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener{
    private:
        Ogre::SceneManager* mSceneMgr;
        Ogre::Camera* mCamera;
        Ogre::SceneNode* mCameraNode;

        // Ajout de la Factory pour créer les objets
        std::unique_ptr<ObjectFactory> mObjectFactory;
        std::unique_ptr<BowlingLane> mLane;
        std::unique_ptr<BowlingBall> mBall;
        std::unique_ptr<CameraController> mCameraController;

        // Ajout du clavier
        const OgreBites::KeyboardEvent* mKeyboard;

        bool mKeyW, mKeyA, mKeyS, mKeyD, mKeySpace, mKeyC;

    public:
        Application();
        virtual ~Application();

        void setup() override;
        bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
        bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
        bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
        bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
        bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
        bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;

        // Getters (utiles pour le pattern Singleton)
        Ogre::SceneManager* getSceneManager() { return mSceneMgr; }
        Ogre::Camera* getCamera() { return mCamera; }

        void createDynamicObjects();

    private:
        void createScene();
        void setupPhysics();
};
********************************************************************************
#include "../../include/core/Application.h"
#include "../../include/managers/PhysicsManager.h" 
#include "../../include/objects/ObjectFactory.h"
#include "../../include/objects/BowlingLane.h"

Application::Application()
    : OgreBites::ApplicationContext("Environnement Dynamique Ogre3D 14.3"),
      mSceneMgr(nullptr),
      mCamera(nullptr),
      mCameraNode(nullptr),
      mKeyW(false), mKeyA(false), mKeyS(false), mKeyD(false), mKeySpace(false), mKeyC(false)
{}

Application::~Application(){}

void Application::setup(){
    // Configuration de base
    OgreBites::ApplicationContext::setup();
    addInputListener(this);
    
    // Création du SceneManager
    Ogre::Root* root = getRoot();
    mSceneMgr = root->createSceneManager();
    
    // Configuration du shader system
    Ogre::RTShader::ShaderGenerator* shadergen = 
        Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(mSceneMgr);
    
    // Création de la caméra
    mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mCamera = mSceneMgr->createCamera("MainCamera");
    mCameraNode->attachObject(mCamera);
    mCameraNode->setPosition(Ogre::Vector3(15, 5, -85));
    mCameraNode->lookAt(Ogre::Vector3(-3, 10, 80), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
    mCameraNode->yaw(Ogre::Degree(180));
    
    mCamera->setNearClipDistance(1);
    
    // Configuration du viewport
    Ogre::Viewport* vp = getRenderWindow()->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
    //mCamera->setFOVy(Ogre::Degree(45));
    
    // Initialisation du contrôleur de caméra
    mCameraController = std::make_unique<CameraController>(mCamera, mSceneMgr);

    // Configuration de la physique AVANT la création de la scène
    setupPhysics();

    // Configurer l'input
    //OgreBites::InputListenerChain listeners;
    //listeners.push_back(this);
    root->addFrameListener(this);
    //root->addInputListenerChain(listeners);
    
    // Création de la scène
    createScene();
}

void Application::createScene(){
    // Lumière ambiante
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));
    
    // Création d'une lumière directionnelle avec SceneNode (méthode moderne)
    Ogre::SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    lightNode->attachObject(light);
    lightNode->setDirection(Ogre::Vector3(-0.5, -0.5, -0.5));
    
    // Ajout d'une deuxième lumière pour mieux éclairer la scène
    Ogre::SceneNode* lightNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light2 = mSceneMgr->createLight("SecondLight");
    light2->setType(Ogre::Light::LT_DIRECTIONAL);
    lightNode2->attachObject(light2);
    lightNode2->setDirection(Ogre::Vector3(0.5, -0.5, 0.5));
    
    // Création de la piste de bowling
    mLane = std::make_unique<BowlingLane>(mSceneMgr);
    mLane->create(Ogre::Vector3(17.0f, 0.0f, 0.0f));

    // Création de la boule de bowling
    mBall = std::make_unique<BowlingBall>(mSceneMgr, "ball.mesh");
    
    // Position initiale de la boule (au début de la piste, légèrement surélevée)
    Ogre::Vector3 ballPosition(14.0f, 3.0f, -69.0f); 
    mBall->create(ballPosition);
}

void Application::createDynamicObjects(){
    // Création d'objets individuels à des positions plus visibles
    mObjectFactory->createDynamicBox(Ogre::Vector3(10, 5, 0), "Box1");
    mObjectFactory->createDynamicBox(Ogre::Vector3(3, 7, 0), "Box2");
    mObjectFactory->createDynamicBox(Ogre::Vector3(-3, 9, 0), "Box3");
    
    mObjectFactory->createDynamicSphere(Ogre::Vector3(5, 6, 0), "Sphere1");
    mObjectFactory->createDynamicSphere(Ogre::Vector3(-5, 8, 0), "Sphere2");
    
    // Création d'une pile plus visible
    mObjectFactory->createStack(5, Ogre::Vector3(10, 0, 0));
}

void Application::setupPhysics(){
    // Initialisation du gestionnaire de physique (Singleton)
    PhysicsManager::getInstance()->initialize(mSceneMgr);
    
    // Création de la Factory
    //mObjectFactory = std::make_unique<ObjectFactory>(mSceneMgr);
    
    // Création du sol
    //mObjectFactory->createStaticGround();
    
    // Création d'objets dynamiques
    //createDynamicObjects();
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt){
    // Mise à jour de la simulation physique
    PhysicsManager::getInstance()->update(evt.timeSinceLastFrame);
    
    // Mise à jour de la boule de bowling
    if (mBall) {
        mBall->update(evt.timeSinceLastFrame);
    }
    
    // Mise à jour de la piste et des quilles
    if (mLane) {
        mLane->update(evt.timeSinceLastFrame);
    }

    // Mise à jour du contrôleur de caméra
    mCameraController->update(evt);

    // Gestion des touches pour déplacer la caméra
    const Ogre::Real moveSpeed = 200.0f;
    Ogre::Vector3 camMove = Ogre::Vector3::ZERO;
    
    if (mKeyW)
        camMove.z -= moveSpeed * evt.timeSinceLastFrame;
    if (mKeyS)
        camMove.z += moveSpeed * evt.timeSinceLastFrame;
    if (mKeyA)
        camMove.x -= moveSpeed * evt.timeSinceLastFrame;
    if (mKeyD)
        camMove.x += moveSpeed * evt.timeSinceLastFrame;
    if (mKeySpace)
        camMove.y += moveSpeed * evt.timeSinceLastFrame;
    if (mKeyC)
        camMove.y -= moveSpeed * evt.timeSinceLastFrame;
    
    mCameraNode->translate(camMove, Ogre::Node::TS_LOCAL);

    // Log de la position de la caméra même si elle ne bouge pas
    Ogre::Vector3 camPos = mCameraNode->getPosition();
    Ogre::LogManager::getSingleton().logMessage("Position de la caméra : " + Ogre::StringConverter::toString(camPos));
    
    return true;
}

// Modifiez la gestion des touches pour ajouter la réinitialisation des quilles
bool Application::keyPressed(const OgreBites::KeyboardEvent& evt){
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE){
        getRoot()->queueEndRendering();
        return true;
    }

    // Lancement de la boule avec la touche Espace (uniquement si la boule n'est pas déjà en mouvement)
    if (evt.keysym.sym == OgreBites::SDLK_SPACE && mBall && !mBall->isRolling()){
        // Direction vers l'avant (le long de l'axe Z négatif)
        Ogre::Vector3 direction(0, 0, -1);
        
        // Puissance du lancer (ajustez selon vos besoins)
        float power = 15.0f;
        
        // Effet de rotation (0 = pas d'effet, valeurs positives/négatives pour effet gauche/droite)
        float spin = 0.0f;
        
        // Lancement de la boule
        mBall->launch(direction, power, spin);
    }

    // Réinitialisation de la boule avec la touche R
    if ((evt.keysym.sym == 'r' || evt.keysym.sym == 'R') && mBall){
        mBall->reset();
        
        // Également réinitialiser les quilles
        if (mLane) {
            mLane->resetPins();
        }
    }
    
    // Mise à jour de l'état des touches
    if (evt.keysym.sym == OgreBites::SDLK_UP || evt.keysym.sym == 'w' || evt.keysym.sym == 'W' || evt.keysym.sym == 'z' || evt.keysym.sym == 'Z')
        mKeyW = true;
    if (evt.keysym.sym == OgreBites::SDLK_DOWN || evt.keysym.sym == 's' || evt.keysym.sym == 'S')
        mKeyS = true;
    if (evt.keysym.sym == OgreBites::SDLK_LEFT || evt.keysym.sym == 'a' || evt.keysym.sym == 'A' || evt.keysym.sym == 'q' || evt.keysym.sym == 'Q')
        mKeyA = true;
    if (evt.keysym.sym == OgreBites::SDLK_RIGHT || evt.keysym.sym == 'd' || evt.keysym.sym == 'D')
        mKeyD = true;
    if (evt.keysym.sym == OgreBites::SDLK_SPACE)
        mKeySpace = true;
    if (evt.keysym.sym == 'c' || evt.keysym.sym == 'C')
        mKeyC = true;
    
    return true;
}

bool Application::keyReleased(const OgreBites::KeyboardEvent& evt){
    // Mise à jour de l'état des touches
    if (evt.keysym.sym == OgreBites::SDLK_UP || evt.keysym.sym == 'w' || evt.keysym.sym == 'W' || evt.keysym.sym == 'z' || evt.keysym.sym == 'Z')
        mKeyW = false;
    if (evt.keysym.sym == OgreBites::SDLK_DOWN || evt.keysym.sym == 's' || evt.keysym.sym == 'S')
        mKeyS = false;
    if (evt.keysym.sym == OgreBites::SDLK_LEFT || evt.keysym.sym == 'a' || evt.keysym.sym == 'A' || evt.keysym.sym == 'q' || evt.keysym.sym == 'Q')
        mKeyA = false;
    if (evt.keysym.sym == OgreBites::SDLK_RIGHT || evt.keysym.sym == 'd' || evt.keysym.sym == 'D')
        mKeyD = false;
    if (evt.keysym.sym == OgreBites::SDLK_SPACE)
        mKeySpace = false;
    if (evt.keysym.sym == 'c' || evt.keysym.sym == 'C')
        mKeyC = false;
    
    return true;
}

bool Application::mousePressed(const OgreBites::MouseButtonEvent& evt) {
    //mCameraController->handleMousePressed(evt);
    mCameraController->handleMouseClick(evt);
    return true;
}

bool Application::mouseMoved(const OgreBites::MouseMotionEvent& evt) {
    //mCameraController->handleMouseMoved(evt);
    return true;
}

bool Application::mouseReleased(const OgreBites::MouseButtonEvent& evt) {
    if (evt.button == OgreBites::BUTTON_RIGHT) {
       // mCameraController->handleMousePressed(evt); // Réutiliser pour arrêter la rotation
    }
    return true;
}
**************************************************************************************
#include "../../include/core/AimingSystem.h"
#include <OgreMaterialManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreResourceGroupManager.h>
#include <OgreStringConverter.h>
#include <algorithm>

AimingSystem::AimingSystem(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera)
    : mSceneMgr(sceneMgr),
      mCamera(camera),
      mArrowNode(nullptr),
      mArrowEntity(nullptr),
      mGameOverlay(nullptr),
      mPowerBarContainer(nullptr),
      mPowerBarBackground(nullptr),
      mPowerBarFill(nullptr),
      mSpinEffectControl(nullptr),
      mSpinEffectIndicator(nullptr),
      mCancelButton(nullptr),
      mScoreContainer(nullptr),
      mScoreDisplay(nullptr),
      mAimingActive(false),
      mAimingDirection(Ogre::Vector3(0, 0, -1)),
      mPowerBarActive(false),
      mPowerValue(0.0f),
      mPowerDirection(1),
      mPowerSpeed(50.0f),
      mSpinEffect(0.0f)
{
}

AimingSystem::~AimingSystem(){
    // Nettoyage des overlays
    if (mGameOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(mGameOverlay);
    }
    
    // Nettoyage des nœuds de scène
    if (mArrowNode) {
        mArrowNode->detachAllObjects();
        mSceneMgr->destroySceneNode(mArrowNode);
    }
    
    // Nettoyage des entités
    if (mArrowEntity) {
        mSceneMgr->destroyEntity(mArrowEntity);
    }
}

void AimingSystem::initialize(){
    // Création des éléments visuels
    createAimingArrow();
    createPowerBar();
    createSpinEffectControl();
    createCancelButton();
    
    // Activation du système de visée
    setAimingActive(true);
}

void createCoordinateAxes(Ogre::SceneManager* sceneMgr, float length = 500.0f) {
    // Créer un objet manuel pour dessiner les axes
    Ogre::ManualObject* axesObject = sceneMgr->createManualObject("CoordinateAxes");
    
    // Mode non-éclairé pour une meilleure visibilité
    axesObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
    
    // Axe X - Rouge
    // axesObject->colour(Ogre::ColourValue(1.0f, 0.0f, 0.0f));
    // axesObject->position(-10, 5, 0);
    // axesObject->position(200, 15, 0);
    
    // Axe Y - Vert
    // axesObject->colour(Ogre::ColourValue(0.0f, 1.0f, 0.0f));
    // axesObject->position(0, 5, 0);
    // axesObject->position(0, 500, 0);
    
    // // Axe Z - Bleu
    axesObject->colour(Ogre::ColourValue(0.0f, 0.0f, 1.0f));
    axesObject->position(0, 5, 0);
    axesObject->position(0, 0, length);
    
    axesObject->end();
    
    // Créer un nœud pour contenir les axes
    Ogre::SceneNode* axesNode = sceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode");
    axesNode->attachObject(axesObject);
}

void AimingSystem::createAimingArrow(){
    // Création d'une flèche directionnelle simple
    // Utilisation d'un cône pour représenter la flèche
    mArrowEntity = mSceneMgr->createEntity("AimingArrow", "cone.mesh");
    
    // Création d'un matériau pour la flèche
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        "AimingArrowMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* technique = material->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    
    pass->setDiffuse(Ogre::ColourValue(1.0f, 0.5f, 0.0f, 0.7f));  // Orange semi-transparent
    pass->setSpecular(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
    pass->setShininess(30);
    pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    pass->setDepthWriteEnabled(false);
    
    mArrowEntity->setMaterialName("AimingArrowMaterial");
    
    // Création du nœud pour la flèche
    mArrowNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("AimingArrowNode");
    mArrowNode->attachObject(mArrowEntity);
    
    // Orientation et mise à l'échelle de la flèche
    mArrowNode->setScale(0.5f, 2.0f, 0.5f);
    mArrowNode->pitch(Ogre::Degree(90));  // Orienter la flèche vers l'avant
    
    // Positionnement initial de la flèche (à ajuster selon la position de la boule)
    mArrowNode->setPosition(14.0f, 1.5f, -69.0f);  // Légèrement au-dessus du sol
}

void AimingSystem::createPowerBar(){
    // Création de l'overlay pour l'interface utilisateur
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    if (!mGameOverlay) {
        mGameOverlay = overlayManager.create("GameOverlay");
    }
    
    // Création du conteneur pour la barre de puissance
    mPowerBarContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "PowerBarContainer"));
    mPowerBarContainer->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarContainer->setPosition(20, 100);
    mPowerBarContainer->setDimensions(30, 200);
    
    // Création de l'arrière-plan de la barre de puissance
    mPowerBarBackground = overlayManager.createOverlayElement("Panel", "PowerBarBackground");
    mPowerBarBackground->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarBackground->setPosition(0, 0);
    mPowerBarBackground->setDimensions(30, 200);
    
    // Création du matériau pour l'arrière-plan
    Ogre::MaterialPtr backgroundMaterial = Ogre::MaterialManager::getSingleton().create("PowerBarBackgroundMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);    

        // Obtention de la technique et du pass
    Ogre::Technique* backgroundTechnique = backgroundMaterial->getTechnique(0);
    Ogre::Pass* backgroundPass = backgroundTechnique->getPass(0);
    
    backgroundPass->setDiffuse(Ogre::ColourValue(0.2f, 0.2f, 0.2f, 0.8f));
    backgroundPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    backgroundPass->setDepthWriteEnabled(false);
    
    mPowerBarBackground->setMaterialName("PowerBarBackgroundMaterial");

    // Création de la barre de remplissage
    mPowerBarFill = overlayManager.createOverlayElement("Panel", "PowerBarFill");
    mPowerBarFill->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarFill->setPosition(0, 200);  // Commence en bas
    mPowerBarFill->setDimensions(30, 0);  // Hauteur initiale à 0
    
    // Création du matériau pour la barre de remplissage
    Ogre::MaterialPtr fillMaterial = Ogre::MaterialManager::getSingleton().create(
        "PowerBarFillMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* fillTechnique = fillMaterial->getTechnique(0);
    Ogre::Pass* fillPass = fillTechnique->getPass(0);
    
    fillPass->setDiffuse(Ogre::ColourValue(0.0f, 1.0f, 0.0f, 0.8f));  // Vert
    fillPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    fillPass->setDepthWriteEnabled(false);
    
    // Assemblage de la barre de puissance
    mPowerBarContainer->addChild(mPowerBarBackground);
    mPowerBarContainer->addChild(mPowerBarFill);
    
    mPowerBarFill->setMaterialName("PowerBarFillMaterial");

    // Ajout à l'overlay principal
    mGameOverlay->add2D(mPowerBarContainer);
    
    // Masquer la barre de puissance initialement
    mPowerBarContainer->hide();
    
    // Affichage de l'overlay
    mGameOverlay->setZOrder(100);
    mGameOverlay->show();
}

void AimingSystem::createSpinEffectControl(){
    // Création du conteneur pour le contrôle d'effet
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    // Création du conteneur pour le curseur d'effet
    Ogre::OverlayContainer* spinContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "SpinEffectContainer"));
    spinContainer->setMetricsMode(Ogre::GMM_PIXELS);
    spinContainer->setPosition(70, 100);
    spinContainer->setDimensions(200, 30);
    
    // Création de l'arrière-plan du curseur d'effet
    mSpinEffectControl = overlayManager.createOverlayElement("Panel", "SpinEffectControl");
    mSpinEffectControl->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinEffectControl->setPosition(0, 0);
    mSpinEffectControl->setDimensions(200, 30);
    
    // Création du matériau pour l'arrière-plan du curseur
    Ogre::MaterialPtr spinBackgroundMaterial = Ogre::MaterialManager::getSingleton().create(
        "SpinEffectBackgroundMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* spinBackgroundTechnique = spinBackgroundMaterial->getTechnique(0);
    Ogre::Pass* spinBackgroundPass = spinBackgroundTechnique->getPass(0);
    
    spinBackgroundPass->setDiffuse(Ogre::ColourValue(0.2f, 0.2f, 0.2f, 0.8f));
    spinBackgroundPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    spinBackgroundPass->setDepthWriteEnabled(false);

    mSpinEffectControl->setMaterialName("SpinEffectBackgroundMaterial");
    
    // Création de l'indicateur du curseur d'effet
    mSpinEffectIndicator = overlayManager.createOverlayElement("Panel", "SpinEffectIndicator");
    mSpinEffectIndicator->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinEffectIndicator->setPosition(95, 0);  // Position centrale
    mSpinEffectIndicator->setDimensions(10, 30);
    
    // Création du matériau pour l'indicateur du curseur
    Ogre::MaterialPtr spinIndicatorMaterial = Ogre::MaterialManager::getSingleton().create(
        "SpinEffectIndicatorMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* spinIndicatorTechnique = spinIndicatorMaterial->getTechnique(0);
    Ogre::Pass* spinIndicatorPass = spinIndicatorTechnique->getPass(0);
    
    spinIndicatorPass->setDiffuse(Ogre::ColourValue(1.0f, 0.0f, 0.0f, 0.8f));  // Rouge
    spinIndicatorPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    spinIndicatorPass->setDepthWriteEnabled(false);

    mSpinEffectIndicator->setMaterialName("SpinEffectIndicatorMaterial");
    
    // Assemblage du curseur d'effet
    spinContainer->addChild(mSpinEffectControl);
    spinContainer->addChild(mSpinEffectIndicator);
    
    // Ajout à l'overlay principal
    mGameOverlay->add2D(spinContainer);
}

void AimingSystem::createCancelButton(){
    // Création du bouton d'annulation
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    // Création du conteneur pour le bouton
    Ogre::OverlayContainer* cancelContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "CancelButtonContainer"));
    cancelContainer->setMetricsMode(Ogre::GMM_PIXELS);
    cancelContainer->setPosition(20, 320);
    cancelContainer->setDimensions(100, 40);
    
    // Création du bouton
    mCancelButton = overlayManager.createOverlayElement("Panel", "CancelButton");
    mCancelButton->setMetricsMode(Ogre::GMM_PIXELS);
    mCancelButton->setPosition(0, 0);
    mCancelButton->setDimensions(100, 40);
    
    // Création du matériau pour le bouton
    Ogre::MaterialPtr cancelButtonMaterial = Ogre::MaterialManager::getSingleton().create(
        "CancelButtonMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* cancelButtonTechnique = cancelButtonMaterial->getTechnique(0);
    Ogre::Pass* cancelButtonPass = cancelButtonTechnique->getPass(0);
    
    cancelButtonPass->setDiffuse(Ogre::ColourValue(0.8f, 0.0f, 0.0f, 0.8f));  // Rouge
    cancelButtonPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    cancelButtonPass->setDepthWriteEnabled(false);

    mCancelButton->setMaterialName("CancelButtonMaterial");
    
    // Création du texte pour le bouton
    Ogre::TextAreaOverlayElement* cancelText = static_cast<Ogre::TextAreaOverlayElement*>(
        overlayManager.createOverlayElement("TextArea", "CancelButtonText"));
    cancelText->setMetricsMode(Ogre::GMM_PIXELS);
    cancelText->setPosition(20, 10);
    cancelText->setDimensions(60, 20);
    cancelText->setCharHeight(16);
    cancelText->setFontName("Arial");
    cancelText->setColour(Ogre::ColourValue::White);
    cancelText->setCaption("Annuler");
    
    // Assemblage du bouton
    cancelContainer->addChild(mCancelButton);
    cancelContainer->addChild(cancelText);
    
    // Ajout à l'overlay principal
    mGameOverlay->add2D(cancelContainer);
    
    // Création du conteneur pour l'affichage du score
    mScoreContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "ScoreContainer"));
    mScoreContainer->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreContainer->setPosition(20, 20);
    mScoreContainer->setDimensions(100, 30);
    
    // Création de l'affichage du score
    mScoreDisplay = static_cast<Ogre::TextAreaOverlayElement*>(
        overlayManager.createOverlayElement("TextArea", "ScoreDisplay"));
    mScoreDisplay->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreDisplay->setPosition(0, 0);
    mScoreDisplay->setDimensions(100, 30);
    mScoreDisplay->setCharHeight(24);
    mScoreDisplay->setFontName("Arial");
    mScoreDisplay->setColour(Ogre::ColourValue::White);
    mScoreDisplay->setCaption("Score: 0");
    
    // Ajout du texte au conteneur
    mScoreContainer->addChild(mScoreDisplay);
    
    // Ajout du conteneur à l'overlay principal
    mGameOverlay->add2D(mScoreContainer);
}

void AimingSystem::update(float deltaTime)
{
    if (mAimingActive) {
        updateAimingArrow();
        
        if (mPowerBarActive) {
            updatePowerBar(deltaTime);
        }
        
        updateSpinEffectControl();
    }
}

void AimingSystem::updateAimingArrow(){
    // Mise à jour de la position et de l'orientation de la flèche
    if (mArrowNode) {
        // La flèche pointe dans la direction de visée
        Ogre::Vector3 direction = mAimingDirection.normalisedCopy();
        
        // Calcul de la rotation pour orienter la flèche
        Ogre::Quaternion rotation = Ogre::Vector3::UNIT_Y.getRotationTo(direction);
        mArrowNode->setOrientation(rotation);
        
        // La flèche reste à une position fixe (à ajuster selon la position de la boule)
        // mArrowNode->setPosition(...);
    }
}

void AimingSystem::updatePowerBar(float deltaTime){
    // Mise à jour de la barre de puissance
    if (mPowerBarActive && mPowerBarFill) {
        // Mise à jour de la valeur de puissance
        mPowerValue += mPowerDirection * mPowerSpeed * deltaTime;
        
        // Inversion de la direction si les limites sont atteintes
        if (mPowerValue >= MAX_POWER) {
            mPowerValue = MAX_POWER;
            mPowerDirection = -1;
        } else if (mPowerValue <= MIN_POWER) {
            mPowerValue = MIN_POWER;
            mPowerDirection = 1;
        }
        
        // Mise à jour de l'affichage de la barre
        float fillHeight = (mPowerValue / MAX_POWER) * 200.0f;
        mPowerBarFill->setPosition(0, 200.0f - fillHeight);
        mPowerBarFill->setDimensions(30, fillHeight);
        
        // Changement de couleur en fonction de la puissance
        Ogre::MaterialPtr fillMaterial = Ogre::MaterialManager::getSingleton().getByName("PowerBarFillMaterial");
        if (fillMaterial) {
            // Obtention de la technique et du pass
            Ogre::Pass* pass = fillMaterial->getTechnique(0)->getPass(0);
            
            // Transition de vert à rouge en fonction de la puissance
            float ratio = mPowerValue / MAX_POWER;
            pass->setDiffuse(Ogre::ColourValue(ratio, 1.0f - ratio, 0.0f, 0.8f));
        }
    }
}

void AimingSystem::updateSpinEffectControl(){
    // Mise à jour de la position de l'indicateur d'effet
    if (mSpinEffectIndicator) {
        // Conversion de la valeur d'effet (-1.0 à 1.0) en position (0 à 200)
        float position = ((mSpinEffect + 1.0f) / 2.0f) * 190.0f;
        mSpinEffectIndicator->setPosition(position, 0);
    }
}

void AimingSystem::handleMouseMove(const OgreBites::MouseMotionEvent& evt) {
    if (mAimingActive) {
        // Exemple : mise à jour de la direction de visée en fonction du mouvement de la souris
        float deltaX = evt.xrel * 0.01f; // Sensibilité horizontale
        float deltaY = evt.yrel * 0.01f; // Sensibilité verticale

        // Ajuster la direction de visée
        mAimingDirection.x += deltaX;
        mAimingDirection.y -= deltaY;

        // Normaliser la direction pour éviter des valeurs incorrectes
        mAimingDirection.normalise();

        // Mettre à jour la flèche de visée
        updateAimingArrow();
    }
}

void AimingSystem::setAimingActive(bool active) {
    mAimingActive = active;
}

Ogre::Vector3 AimingSystem::getAimingDirection() const {
    return mAimingDirection;
}

float AimingSystem::getPower() const {
    return mPowerValue;
}

float AimingSystem::getSpinEffect() const {
    return mSpinEffect;
}

void AimingSystem::resetAiming() {
    mAimingDirection = Ogre::Vector3::ZERO;
    mPowerValue = 0.0f;
    mSpinEffect = 0.0f;
    mAimingActive = false;
}

void AimingSystem::startPowerBar() {
    mPowerBarActive = true;
    mPowerBarContainer->show();
}

/*
void AimingSystem::handleMouseMove(const OgreBites::MouseMotionEvent& evt)
{
    if (mAimingActive) {
        // Gestion du mouvement de la souris pour la visée
        // Conversion des coordonnées de la souris en rayon dans l'espace 3D
        Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
            evt.x / (float)mCamera->getViewport()->getActualWidth(),
            evt.y / (float)mCamera->getViewport()->getActualHeight());
        
        // Intersection avec le plan du sol (y = 0)
        Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0);
        std::pair<bool, Ogre::Real> intersection = mouseRay.intersects(groundPlane);
        
        if (intersection.first) {
            // Point d'intersection avec le sol
            Ogre::Vector3 hitPoint = mouseRay.getPoint(intersection.second);
            
            // Calcul de la direction de visée (depuis la position de la boule vers le point d'intersection)
            Ogre::Vector3 ballPosition(14.0f, 1.5f, -69.0f);  // À ajuster selon la position réelle de la boule
            Ogre::Vector3 direction = (hitPoint - ballPosition).normalisedCopy();
            
            // Mise à jour de la direction de visée
            setAimingDirection(direction);
        }
    }
}
        // Gestion du curseur d'effet
        // V
*/
