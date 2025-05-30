#include "../../include/core/Application.h"
#include "../../include/managers/PhysicsManager.h" 
#include "../../include/objects/ObjectFactory.h"
#include "../../include/objects/BowlingLane.h"
#include "../../include/core/GameManager.h"

Application::Application()
    : OgreBites::ApplicationContext("Crazy Bowling !!"),
      scene(nullptr),
      mCamera(nullptr),
      mCameraNode(nullptr),
      mKeyW(false), mKeyA(false), mKeyS(false), mKeyD(false), mKeySpace(false), mKeyC(false),
      overlaySystem(nullptr)
{}

Application::~Application(){}

void Application::checkFMOD(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}

void Application::initAudio() {
    checkFMOD(FMOD::System_Create(&fmodSystem));
    checkFMOD(fmodSystem->init(512, FMOD_INIT_NORMAL, 0));

    // Charger un son (WAV, MP3, etc.)
    checkFMOD(fmodSystem->createSound("bowling-strike.wav", FMOD_DEFAULT, 0, &sound));
}

void Application::playSound() {
    checkFMOD(fmodSystem->playSound(sound, 0, false, &channel));
}

void Application::updateAudio() {
    fmodSystem->update();
}

void Application::cleanAudio() {
    sound->release();
    fmodSystem->close();
    fmodSystem->release();
}

void Application::initFMOD() {
    checkFMOD(FMOD::System_Create(&fmodSystem));
    checkFMOD(fmodSystem->init(512, FMOD_INIT_NORMAL, nullptr));

    // Charger le son en mode boucle
    checkFMOD(fmodSystem->createSound("bowling-strike.wav", FMOD_LOOP_NORMAL, 0, &sound));
    
    // Jouer le son en boucle immédiatement
    checkFMOD(fmodSystem->playSound(sound, nullptr, false, &channel));
}

void Application::updateFMOD() {
    fmodSystem->update();
}

void Application::shutdownFMOD() {
    sound->release();
    fmodSystem->close();
    fmodSystem->release();
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
    mCameraNode = scene->getRootSceneNode()->createChildSceneNode();
    mCamera = scene->createCamera("MainCamera");
    mCameraNode->attachObject(mCamera);
    mCameraNode->setPosition(Ogre::Vector3(0.0f, 2.0f, -13.0f));
    mCameraNode->lookAt(Ogre::Vector3(-3, 1, 80), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
    mCameraNode->yaw(Ogre::Degree(180));
    
    mCamera->setNearClipDistance(1);
    
    // Configuration du viewport
    Ogre::Viewport* vp = getRenderWindow()->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2));
    vp = getRenderWindow()->getViewport(0);
    if (vp) {
        vp->setOverlaysEnabled(true);
    }
    std::cout << "Overlays activés pour le viewport : " << vp->getOverlaysEnabled() << std::endl;
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    // Configuration de la physique AVANT la création de la scène
    setupPhysics();

    // Configurer l'input
    root->addFrameListener(this);

    // Ajout des overlays
    overlaySystem = Ogre::OverlaySystem::getSingletonPtr();
    scene->addRenderQueueListener(overlaySystem);
    
    // Création de la scène
    createScene();

    // Initialisation du gestionnaire de jeu
    GameManager::getInstance()->initialize(scene, mCamera, mBall.get(), mLane.get());
}

void Application::createScene(){
    // Lumière ambiante
    scene->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    
    // Création d'une lumière directionnelle avec SceneNode (méthode moderne)
    Ogre::SceneNode* lightNode = scene->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light = scene->createLight("MainLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    lightNode->attachObject(light);
    lightNode->setDirection(Ogre::Vector3(3, 5, 80));
    
    // Ajout d'une deuxième lumière pour mieux éclairer la scène
    Ogre::SceneNode* lightNode2 = scene->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light2 = scene->createLight("SecondLight");
    light2->setType(Ogre::Light::LT_DIRECTIONAL);
    lightNode2->attachObject(light2);
    lightNode2->setDirection(Ogre::Vector3(14, 5, -15));
    
    // Création de la piste de bowling
    mLane = std::make_unique<BowlingLane>(scene);
    mLane->create(Ogre::Vector3(0.0f, 0.5f, -7.5f));

    // Création de la boule de bowling
    mBall = std::make_unique<BowlingBall>(scene, "ball.mesh");
    
    // Position initiale de la boule (au début de la piste, légèrement surélevée)
    Ogre::Vector3 ballPosition(0.0f, 0.25f, -7.5f); 
    mBall->create(ballPosition);
}

void Application::setupPhysics(){
    // Initialisation du gestionnaire de physique (Singleton)
    PhysicsManager::getInstance()->initialize(scene);

    // Activer ou desactiver le debugDrawer
    PhysicsManager::getInstance()->toggleDebugDrawing();
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt){
     // Mise à jour du gestionnaire de jeu
     GameManager::getInstance()->update(evt.timeSinceLastFrame);

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

    if (mKeyC)
        camMove.y -= moveSpeed * evt.timeSinceLastFrame;
    
    mCameraNode->translate(camMove, Ogre::Node::TS_LOCAL);

    updateFMOD();

    return true;
}

bool Application::keyPressed(const OgreBites::KeyboardEvent& evt){
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE){
        shutdownFMOD();
        getRoot()->queueEndRendering();
        return true;
    }

    // Transmission de l'événement au gestionnaire de jeu
    if (GameManager::getInstance()->handleKeyPress(evt)) {
        return true;
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
    if (evt.keysym.sym == 'r' || evt.keysym.sym == 'R') {
        GameManager::getInstance()->resetGame();
    }
    
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
