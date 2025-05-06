// GameManager.cpp - Implémentation de la gestion du jeu
#include "GameManager.h"
#include <OgreEntity.h>
#include <OgreLight.h>
#include <OgreMaterialManager.h>

GameManager::GameManager(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, Ogre::RenderWindow* window)
    : mSceneMgr(sceneMgr)
    , mCamera(camera)
    , mWindow(window)
    , mDynamicsWorld(nullptr)
    , mDebugDrawer(nullptr)
    , mPlayer(nullptr)
    , mLevel(nullptr)
    , mGameState(GAME_RUNNING)
    , mGameTime(0.0f)
    , mPlayerDetected(false)
    , mObjectiveReached(false)
{
    // Initialiser les touches
    for (int i = 0; i < 256; i++) {
        mKeysPressed[i] = false;
    }
}

GameManager::~GameManager()
{
    // Nettoyer les corps physiques
    for (auto it = mBodies.begin(); it != mBodies.end(); ++it) {
        delete *it;
    }
    mBodies.clear();

    // Nettoyer les formes de collision
    for (auto it = mShapes.begin(); it != mShapes.end(); ++it) {
        delete *it;
    }
    mShapes.clear();

    // Nettoyer les objets du jeu
    delete mPlayer;
    for (auto it = mGuards.begin(); it != mGuards.end(); ++it) {
        delete *it;
    }
    mGuards.clear();
    delete mLevel;

    // Nettoyer la physique
    delete mDynamicsWorld;
    delete mDebugDrawer;
}

void GameManager::setupGame()
{
    // Configurer l'éclairage
    setupLights();

    // Configurer la physique
    setupPhysics();

    // Créer le niveau
    mLevel = new Level(this);
    mLevel->createLevel();

    // Créer le joueur
    createPlayer();

    // Créer les gardes
    createGuards();
}

void GameManager::setupPhysics()
{
    // Configurer les paramètres du monde physique
    Ogre::AxisAlignedBox worldBounds = Ogre::AxisAlignedBox(
        Ogre::Vector3(-300, -300, -300),
        Ogre::Vector3(300, 300, 300)
    );
    Ogre::Vector3 gravity(0, -9.8f, 0);

    // Créer le monde de physique
    mDynamicsWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, worldBounds, gravity);
    
    // Créer un debug drawer (optionnel)
    mDebugDrawer = new OgreBulletCollisions::DebugDrawer();
    mDebugDrawer->setDrawWireframe(true);
    mDynamicsWorld->setDebugDrawer(mDebugDrawer);
    mDynamicsWorld->setShowDebugShapes(false);  // Mettre à true pour le débogage
    
    Ogre::SceneNode* debugDrawerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer");
    debugDrawerNode->attachObject(static_cast<Ogre::SimpleRenderable*>(mDebugDrawer));
}

void GameManager::setupLights()
{
    // Lumière ambiante
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
    
    // Lumière directionnelle principale
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(Ogre::Vector3(0.5f, -1.0f, 0.5f).normalisedCopy());
    light->setDiffuseColour(Ogre::ColourValue(0.8f, 0.8f, 0.8f));
    light->setSpecularColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
    
    // Lumière d'appoint
    Ogre::Light* fillLight = mSceneMgr->createLight("FillLight");
    fillLight->setType(Ogre::Light::LT_DIRECTIONAL);
    fillLight->setDirection(Ogre::Vector3(-0.5f, -0.5f, -0.5f).normalisedCopy());
    fillLight->setDiffuseColour(Ogre::ColourValue(0.3f, 0.3f, 0.4f));
    fillLight->setSpecularColour(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
}

void GameManager::createPlayer()
{
    // Position de départ du joueur
    Ogre::Vector3 playerStartPos = mLevel->getPlayerStartPosition();
    
    // Créer le joueur
    mPlayer = new Player(this, "Player", playerStartPos);
    
    // Attacher la caméra au joueur
    mPlayer->attachCamera(mCamera);
}

void GameManager::createGuards()
{
    // Obtenir les positions des gardes du niveau
    std::vector<Ogre::Vector3> guardPositions = mLevel->getGuardPositions();
    std::vector<Ogre::Vector3> guardPatrolPoints = mLevel->getGuardPatrolPoints();
    
    // Créer les gardes
    for (size_t i = 0; i < guardPositions.size(); ++i) {
        Ogre::String guardName = "Guard_" + Ogre::StringConverter::toString(i);
        Guard* guard = new Guard(this, guardName, guardPositions[i]);
        
        // Ajouter des points de patrouille pour ce garde
        // Pour simplifier, on utilise des points communs pour tous les gardes
        guard->setPatrolPath(guardPatrolPoints);
        
        mGuards.push_back(guard);
    }
}

void GameManager::update(float deltaTime)
{
    mGameTime += deltaTime;
    
    // Si le jeu n'est pas en cours, ne pas mettre à jour
    if (mGameState != GAME_RUNNING) {
        return;
    }
    
    // Mettre à jour la physique
    updatePhysics(deltaTime);
    
    // Mettre à jour le joueur
    if (mPlayer) {
        mPlayer->update(deltaTime);
    }
    
    // Mettre à jour les gardes
    for (auto it = mGuards.begin(); it != mGuards.end(); ++it) {
        (*it)->update(deltaTime);
        
        // Vérifier si le garde détecte le joueur
        if ((*it)->detectPlayer(mPlayer)) {
            mPlayerDetected = true;
        }
    }
    
    // Vérifier si l'objectif est atteint
    if (mPlayer && mLevel) {
        if (mPlayer->getPosition().distance(mLevel->getObjectivePosition()) < 2.0f) {
            mObjectiveReached = true;
        }
    }
    
    // Vérifier l'état du jeu
    checkGameStatus();
}

void GameManager::updatePhysics(float deltaTime)
{
    if (mDynamicsWorld) {
        mDynamicsWorld->stepSimulation(deltaTime);
    }
}

void GameManager::checkGameStatus()
{
    // Vérifier si le joueur a été détecté
    if (mPlayerDetected) {
        mGameState = GAME_OVER;
        std::cout << "Game Over! Vous avez été détecté!" << std::endl;
    }
    
    // Vérifier si l'objectif a été atteint
    if (mObjectiveReached) {
        mGameState = GAME_WON;
        std::cout << "Victoire! Vous avez atteint l'objectif!" << std::endl;
    }
}

void GameManager::keyPressed(const OIS::KeyEvent& evt)
{
    mKeysPressed[evt.key] = true;
    
    // Gestion des touches pour le joueur
    if (mPlayer) {
        mPlayer->keyPressed(evt);
    }
}

void GameManager::keyReleased(const OIS::KeyEvent& evt)
{
    mKeysPressed[evt.key] = false;
    
    // Gestion des touches pour le joueur
    if (mPlayer) {
        mPlayer->keyReleased(evt);
    }
}

void GameManager::mouseMoved(const OIS::MouseEvent& evt)
{
    // Gestion de la souris pour le joueur
    if (mPlayer) {
        mPlayer->mouseMoved(evt);
    }
}

void GameManager::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    // Gestion du clic de souris pour le joueur
    if (mPlayer) {
        mPlayer->mousePressed(evt, id);
    }
}

void GameManager::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    // Gestion du relâchement de souris pour le joueur
    if (mPlayer) {
        mPlayer->mouseReleased(evt, id);
    }
}

OgreBulletDynamics::RigidBody* GameManager::createRigidBody(const Ogre::String& name, 
                                                           float mass, 
                                                           const Ogre::Vector3& pos, 
                                                           const Ogre::Quaternion& quat, 
                                                           OgreBulletCollisions::CollisionShape* shape)
{
    // Créer un corps rigide
    OgreBulletDynamics::RigidBody* body = new OgreBulletDynamics::RigidBody(name, mDynamicsWorld);
    
    // Définir la forme et les paramètres
    body->setShape(mSceneMgr->getRootSceneNode(), shape, 0.6f, 0.6f, mass, pos, quat);
    
    // Ajouter aux listes pour le nettoyage ultérieur
    mBodies.push_back(body);
    mShapes.push_back(shape);
    
    return body;
}