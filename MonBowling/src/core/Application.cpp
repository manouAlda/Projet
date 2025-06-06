// Fichier : Application.cpp
#include "../../include/core/Application.h"
#include "../../include/managers/PhysicsManager.h"
#include "../../include/objects/BowlingLane.h"
#include "../../include/core/GameManager.h"
#include "../../include/managers/AudioManager.h" // Ajout de l'AudioManager

Application::Application()
    : OgreBites::ApplicationContext("Crazy Bowling !!"),
      scene(nullptr),
      mCamera(nullptr),
      mCameraNode(nullptr),
      // Retrait des variables mKey* non utilisées pour le déplacement caméra
      // mKeyW(false), mKeyA(false), mKeyS(false), mKeyD(false), mKeySpace(false), mKeyC(false),
      overlaySystem(nullptr)
{}

// Destructeur pour s'assurer que l'AudioManager est arrêté
Application::~Application(){
    // Arrêter l'AudioManager avant de fermer l'application
    AudioManager::getInstance()->shutdown();
}

void Application::setup(){
    // Configuration de base
    OgreBites::ApplicationContext::setup();
    addInputListener(this);

    // Création du SceneManager
    Ogre::Root* root = getRoot();
    scene = root->createSceneManager();

    // Configuration du shader system
    Ogre::RTShader::ShaderGenerator* shadergen =
        Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scene);

    // Création de la caméra
    mCameraNode = scene->getRootSceneNode()->createChildSceneNode("MainCameraNode"); // Nom plus spécifique
    mCamera = scene->createCamera("MainCamera");
    mCameraNode->attachObject(mCamera);
    // Utiliser la position initiale définie dans CameraFollower
    // mCameraNode->setPosition(Ogre::Vector3(0.0f, 3.0f, -18.0f));
    // mCameraNode->lookAt(Ogre::Vector3(-3.0f, 1.5f, 10.0f), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
    // mCameraNode->yaw(Ogre::Degree(180));
    mCamera->setNearClipDistance(1);

    // Configuration du viewport
    Ogre::Viewport* vp = getRenderWindow()->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2));
    // vp = getRenderWindow()->getViewport(0); // vp est déjà le viewport créé
    if (vp) {
        vp->setOverlaysEnabled(true);
        mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
        Ogre::LogManager::getSingleton().logMessage("Overlays activés pour le viewport.");
    } else {
        Ogre::LogManager::getSingleton().logError("Impossible de récupérer le viewport.");
    }


    // Initialisation de l'AudioManager AVANT GameManager
    if (!AudioManager::getInstance()->initialize()) {
        OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "Impossible d'initialiser AudioManager (FMOD)", "Application::setup");
    }

    // Configuration de la physique AVANT la création de la scène
    setupPhysics();

    // Configurer l'input
    root->addFrameListener(this);

    // Ajout des overlays (géré par AimingSystem et ScoreManager maintenant)
    // overlaySystem = Ogre::OverlaySystem::getSingletonPtr();
    // scene->addRenderQueueListener(overlaySystem);

    // Création de la scène
    createScene();

    // Initialisation du gestionnaire de jeu (qui initialisera les autres systèmes)
    GameManager::getInstance()->initialize(scene, mCamera, mBall.get(), mLane.get());

    // La position/orientation initiale de la caméra est maintenant gérée par CameraFollower/GameManager
    // lors de l'initialisation ou du reset.
}

void Application::createScene(){
    // Lumière ambiante
    scene->setAmbientLight(Ogre::ColourValue(0.6, 0.6, 0.6)); // Moins forte pour contraster

    // Création d'une lumière directionnelle
    Ogre::SceneNode* lightNode = scene->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light = scene->createLight("MainLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    lightNode->attachObject(light);
    lightNode->setDirection(Ogre::Vector3(0.5, -1, -0.8).normalisedCopy()); // Direction ajustée
    light->setDiffuseColour(1.0, 1.0, 0.9);
    light->setSpecularColour(0.8, 0.8, 0.7);

    // Ajout d'une lumière ponctuelle au-dessus des quilles (optionnel)
    Ogre::SceneNode* pinLightNode = scene->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* pinLight = scene->createLight("PinLight");
    pinLight->setType(Ogre::Light::LT_SPOTLIGHT);
    pinLightNode->attachObject(pinLight);
    pinLightNode->setPosition(0, 10, 18); // Au-dessus des quilles
    pinLightNode->setDirection(0, -1, 0); // Pointe vers le bas
    pinLight->setSpotlightRange(Ogre::Degree(45), Ogre::Degree(60), 0.9f);
    pinLight->setDiffuseColour(0.7, 0.7, 0.5);

    // Création de la piste de bowling
    mLane = std::make_unique<BowlingLane>(scene);
    mLane->create(Ogre::Vector3(0.0f, 0.0f, 0.0f)); // Position de base à (0,0,0)

    // Création de la boule de bowling
    mBall = std::make_unique<BowlingBall>(scene, "BowlingBall.mesh"); // Utiliser le nom correct du mesh

    // Position initiale de la boule (au début de la piste)
    Ogre::Vector3 ballPosition(0.0f, mBall->getRadius() + 0.01f, -8.0f); // Ajuster Z si besoin
    mBall->create(ballPosition);
}

void Application::setupPhysics(){
    // Initialisation du gestionnaire de physique (Singleton)
    PhysicsManager::getInstance()->initialize(scene);

    // Activer ou desactiver le debugDrawer (optionnel)
    // PhysicsManager::getInstance()->toggleDebugDrawing();
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt){
    // Mise à jour de l'AudioManager (important pour FMOD)
    AudioManager::getInstance()->update();

    // Mise à jour du gestionnaire de jeu
    GameManager::getInstance()->update(evt.timeSinceLastFrame);

    // Mise à jour de la simulation physique
    PhysicsManager::getInstance()->update(evt.timeSinceLastFrame);

    // Les mises à jour de la boule et de la piste sont maintenant gérées par GameManager ou PhysicsManager
    // if (mBall) {
    //     mBall->update(evt.timeSinceLastFrame);
    // }
    // if (mLane) {
    //     mLane->update(evt.timeSinceLastFrame);
    // }

    // Retrait de la gestion manuelle de la caméra ici
    /*
    const Ogre::Real moveSpeed = 200.0f;
    Ogre::Vector3 camMove = Ogre::Vector3::ZERO;
    if (mKeyW) camMove.z -= moveSpeed * evt.timeSinceLastFrame;
    if (mKeyS) camMove.z += moveSpeed * evt.timeSinceLastFrame;
    if (mKeyA) camMove.x -= moveSpeed * evt.timeSinceLastFrame;
    if (mKeyD) camMove.x += moveSpeed * evt.timeSinceLastFrame;
    if (mKeyC) camMove.y -= moveSpeed * evt.timeSinceLastFrame;
    if (mCameraNode) mCameraNode->translate(camMove, Ogre::Node::TS_LOCAL);
    */

    // Affichage de la position de la caméra (peut être utile pour le debug)
    // if (mCameraNode) {
    //     Ogre::Vector3 camPos = mCameraNode->getPosition();
    //     Ogre::LogManager::getSingleton().logMessage("Position de la caméra : " +
    //         Ogre::StringConverter::toString(camPos));
    // }

    // Continuer le rendu
    return OgreBites::ApplicationContext::frameRenderingQueued(evt);
}

// --- Gestion des entrées --- 

bool Application::keyPressed(const OgreBites::KeyboardEvent& evt){
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE){
        getRoot()->queueEndRendering();
        return true;
    }

    // Transmission de l'événement au gestionnaire de jeu
    if (GameManager::getInstance()->handleKeyPress(evt)) {
        return true;
    }

    // Retrait de la mise à jour des mKey* ici

    // La touche R est gérée par GameManager maintenant
    // if (evt.keysym.sym == 'r' || evt.keysym.sym == 'R') {
    //     GameManager::getInstance()->resetGame();
    // }

    return true;
}

bool Application::keyReleased(const OgreBites::KeyboardEvent& evt){
    // Transmission de l'événement au gestionnaire de jeu
    if (GameManager::getInstance()->handleKeyRelease(evt)) {
        return true;
    }

    // Retrait de la mise à jour des mKey* ici

    return true;
}

bool Application::mousePressed(const OgreBites::MouseButtonEvent& evt) {
    // Transmission de l'événement au gestionnaire de jeu
    if (GameManager::getInstance()->handleMousePress(evt)) {
        return true;
    }
    return true;
}

bool Application::mouseMoved(const OgreBites::MouseMotionEvent& evt) {
    // Transmission de l'événement au gestionnaire de jeu
    if (GameManager::getInstance()->handleMouseMove(evt)) {
        return true;
    }
    return true;
}

bool Application::mouseReleased(const OgreBites::MouseButtonEvent& evt) {
    // Transmission de l'événement au gestionnaire de jeu
    if (GameManager::getInstance()->handleMouseRelease(evt)) {
        return true;
    }
    return true;
}

// Surcharge pour gérer la fermeture de la fenêtre
void Application::windowClosed(Ogre::RenderWindow* rw) {
    // S'assurer que le rendu s'arrête proprement
    getRoot()->queueEndRendering();
    // Appeler la méthode de base
    OgreBites::ApplicationContext::windowClosed(rw);
}

// Surcharge pour la fermeture de l'application
void Application::shutdown() {
    // Arrêter l'AudioManager
    AudioManager::getInstance()->shutdown();

    // Appeler la méthode de base pour le reste du nettoyage
    OgreBites::ApplicationContext::shutdown();
}

