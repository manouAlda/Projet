#include "../Header/World.hpp"
#include "../Header/Physics.hpp"
#include "../Header/Player.hpp" 

World::World() : OgreBites::ApplicationContext("NewWorld"), 
                physics(nullptr), 
                sceneMgr(nullptr),
                mouseHandler(nullptr),
                player(nullptr) {
    
}

World::~World() {
    delete player;  // N'oubliez pas de libérer la mémoire
    //delete mouseHandler;
    //delete physics;
}

void World::configurationFenetre() {
    Ogre::RenderSystem* rs = getRoot()->getRenderSystemByName("OpenGL Rendering Subsystem");
    if(!rs) {
        Ogre::LogManager::getSingleton().logMessage("Erreur");
        return;
    }
    getRoot()->setRenderSystem(rs);
    rs->setConfigOption("Full Screen", "No");
    rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
    rs->setConfigOption("VSync", "Yes");
    rs->setConfigOption("FSAA", "4");
}

void World::setupCamera() {
    // Créer une caméra
    mCamera = sceneMgr->createCamera("MainCamera");
    mCamera->setNearClipDistance(5);
    
    // Créer un nœud pour la caméra
    mCameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode("CameraNode");
    mCameraNode->attachObject(mCamera);
    
    // Position de départ de la caméra
    mCameraNode->setPosition(0, 20, 50); // Derrière et au-dessus du joueur
    mCameraNode->lookAt(Ogre::Vector3(0, 10, 0), Ogre::Node::TS_WORLD);
    
    // Activer le suivi du joueur
    mCameraFollowPlayer = true;
    mCameraOffset = Ogre::Vector3(0, 10, 30); // Offset par rapport au joueur
    
    // Attacher la caméra au viewport
    Ogre::RenderWindow* window = getRenderWindow();
    Ogre::Viewport* vp = window->addViewport(mCamera);
}

void World::updateCamera(float deltaTime) {
    if (mCameraFollowPlayer && player) {
        // Obtenir la position actuelle du joueur
        Ogre::Vector3 playerPos = player->getPosition();
        
        // Calculer la position cible de la caméra
        Ogre::Vector3 targetPos = playerPos + mCameraOffset;
        
        // Appliquer un lissage au mouvement de la caméra
        Ogre::Vector3 currentPos = mCameraNode->getPosition();
        float smoothFactor = 5.0f * deltaTime; // Facteur de lissage
        
        Ogre::Vector3 newPos = currentPos + (targetPos - currentPos) * smoothFactor;
        mCameraNode->setPosition(newPos);
        
        // Faire regarder la caméra vers le joueur
        mCameraNode->lookAt(playerPos, Ogre::Node::TS_WORLD);
    }
}

void World::setup() {
    
    configurationFenetre();

    // App setup
    OgreBites::ApplicationContext::setup();
    addInputListener(this);

    // Create SceneManager
    sceneMgr = getRoot()->createSceneManager();
    
    // Ensure the SceneManager is registered with the ShaderGenerator
    Ogre::RTShader::ShaderGenerator* shaderGen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    if (shaderGen) {
        shaderGen->addSceneManager(sceneMgr);
    }
    
    //niveau de base d'éclairage partout
    sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    
    // Initialize Physics
    physics = new Physics(sceneMgr);
    
    // Creer le plan en static
    physics->createStaticGround(physics, sceneMgr);
    
    // Créer quelques objets dynamiques
    physics->createDynamicCube(physics, sceneMgr, Ogre::Vector3(0, 20, 0), "Cube1");
    physics->createDynamicCube(physics, sceneMgr, Ogre::Vector3(5, 30, 5), "Cube2");
    //physics->createDynamicSphere(physics, sceneMgr, Ogre::Vector3(-5, 40, -3), "Sphere1");
    
    // Créer le joueur - position initiale à (0, 10, 0)
    player = new Player(sceneMgr, physics->getDynamicsWorld()->getBtWorld(), Ogre::Vector3(0, 10, 0));

    // Ajout d'un caméra
    Ogre::SceneNode* camNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Camera* cam = sceneMgr->createCamera("MainCamera");
    cam->setNearClipDistance(5);
    camNode->attachObject(cam);
    camNode->setPosition(0, 30, 300);
    camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);
    
    //setupCamera();

    // Ajouter une lumière
    Ogre::Light* light = sceneMgr->createLight("MainLight");
    light->setDiffuseColour(1.0f, 1.0f, 1.0f);
    light->setSpecularColour(1.0f, 1.0f, 1.0f);
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setCastShadows(true);
    
    // Créer MouseHandler
    //mouseHandler = new MouseHandler(cam, camNode);
    //addInputListener(mouseHandler);
    
    // Ajout de la lumière
    Ogre::SceneNode* lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode("LightNode");
    lightNode->attachObject(light);
    lightNode->setDirection(Ogre::Vector3(0, -1, 0));
    
    // Attacher caméra au viewport
    Ogre::RenderWindow* window = getRenderWindow();
    Ogre::Viewport* vp = window->addViewport(cam);
    //vp->setBackgroundColour(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
    //cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

bool World::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    physics->update(evt.timeSinceLastFrame);
    //mouseHandler->update(evt.timeSinceLastFrame);
    
    // Mettre à jour l'état du joueur
    if (player) {
        player->update(evt.timeSinceLastFrame);
    }

    //updateCamera(evt.timeSinceLastFrame);
    
    return OgreBites::ApplicationContext::frameRenderingQueued(evt);
}

// Implémentation des gestionnaires d'entrée clavier
bool World::keyPressed(const OgreBites::KeyboardEvent& evt) {
    if (player) {
        float deltaTime = 0.016f; // approximation pour une réponse immédiate
        
        switch (evt.keysym.sym) {
            
            case OgreBites::SDLK_UP:
                player->moveForward(deltaTime);
                return true;
            
            case OgreBites::SDLK_DOWN:
                player->moveBackward(deltaTime);
                return true;
            
            case OgreBites::SDLK_LEFT:
                player->moveLeft(deltaTime);
                return true;
            
            case OgreBites::SDLK_RIGHT:
                player->moveRight(deltaTime);
                return true;
            case OgreBites::SDLK_SPACE:
                player->jump();
                return true;
            case OgreBites::SDLK_ESCAPE:
                getRoot()->queueEndRendering();
                return true;
        }
    }
    
    return false;
}

bool World::keyReleased(const OgreBites::KeyboardEvent& evt) {
    // Si nécessaire, vous pouvez implémenter des actions lors du relâchement des touches
    return false;
}

