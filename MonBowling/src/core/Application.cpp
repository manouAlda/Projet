#include "../../include/core/Application.h"
#include "../../include/managers/PhysicsManager.h" 
#include "../../include/objects/ObjectFactory.h"
#include "../../include/objects/BowlingLane.h"
#include "../../include/core/GameManager.h"

Application::Application()
    : OgreBites::ApplicationContext("Environnement Dynamique Ogre3D 14.3"),
      mSceneMgr(nullptr),
      mCamera(nullptr),
      mCameraNode(nullptr),
      mKeyW(false), mKeyA(false), mKeyS(false), mKeyD(false), mKeySpace(false), mKeyC(false),
      overlaySystem(nullptr)
{}

Application::~Application(){}

void Application::setup(){
    // Configuration de base
    OgreBites::ApplicationContext::setup();
    addInputListener(this);
    
    // Création du SceneManager
    Ogre::Root* root = getRoot();
    mSceneMgr = root->createSceneManager();

    // Initialisation du système d'overlay
/*    
    overlaySystem = new Ogre::OverlaySystem();
    mSceneMgr->addRenderQueueListener(overlaySystem);
    std::cout << "OverlaySystem initialisé" << std::endl;
*/    
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
    vp = getRenderWindow()->getViewport(0);
    if (vp) {
        vp->setOverlaysEnabled(true);
    }
    std::cout << "Overlays activés pour le viewport : " << vp->getOverlaysEnabled() << std::endl;
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
    
    // Initialisation du contrôleur de caméra
    mCameraController = std::make_unique<CameraController>(mCamera, mSceneMgr);

    // Configuration de la physique AVANT la création de la scène
    setupPhysics();

    // Configurer l'input
    root->addFrameListener(this);
    
    // Création de la scène
    createScene();

    // Initialisation du gestionnaire de jeu
    GameManager::getInstance()->initialize(mSceneMgr, mCamera, mBall.get(), mLane.get());
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
    
    // Mise à jour du gestionnaire de jeu
    GameManager::getInstance()->update(evt.timeSinceLastFrame);

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
/*   
    if (mKeySpace)
        camMove.y += moveSpeed * evt.timeSinceLastFrame;
*/
    if (mKeyC)
        camMove.y -= moveSpeed * evt.timeSinceLastFrame;
    
    mCameraNode->translate(camMove, Ogre::Node::TS_LOCAL);
    
    return true;
}

bool Application::keyPressed(const OgreBites::KeyboardEvent& evt){
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE){
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
    
    // Sinon, transmission au contrôleur de caméra
    mCameraController->handleMouseClick(evt);
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
