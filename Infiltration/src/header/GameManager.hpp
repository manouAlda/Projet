// GameManager.h - Gestion principale du jeu d'infiltration
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreRenderWindow.h>
#include <OIS/OIS.h>

#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletCollisionsShape.h"
#include "OgreBulletDynamicsWorld.h"
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"

#include "Player.h"
#include "Guard.h"
#include "Level.h"

class GameManager
{
private:
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
    Ogre::RenderWindow* mWindow;
    
    // Gestionnaire de physique
    OgreBulletDynamics::DynamicsWorld* mDynamicsWorld;
    OgreBulletCollisions::DebugDrawer* mDebugDrawer;
    std::deque<OgreBulletDynamics::RigidBody*> mBodies;
    std::deque<OgreBulletCollisions::CollisionShape*> mShapes;
    
    // Éléments du jeu
    Player* mPlayer;
    std::vector<Guard*> mGuards;
    Level* mLevel;
    
    // État du jeu
    enum GameState {
        GAME_RUNNING,
        GAME_PAUSED,
        GAME_OVER,
        GAME_WON
    };
    
    GameState mGameState;
    
    // Timer et contrôle du jeu
    float mGameTime;
    bool mKeysPressed[256];
    
    // Indicateurs d'état
    bool mPlayerDetected;
    bool mObjectiveReached;
    
    // Initialisation des composants du jeu
    void setupPhysics();
    void setupLights();
    void createPlayer();
    void createGuards();
    
public:
    GameManager(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, Ogre::RenderWindow* window);
    ~GameManager();
    
    // Initialisation du jeu
    void setupGame();
    
    // Méthodes de mise à jour
    void update(float deltaTime);
    void updatePhysics(float deltaTime);
    void checkGameStatus();
    
    // Gestion des entrées
    void keyPressed(const OIS::KeyEvent& evt);
    void keyReleased(const OIS::KeyEvent& evt);
    void mouseMoved(const OIS::MouseEvent& evt);
    void mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
    void mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
    
    // Accesseurs
    Ogre::SceneManager* getSceneManager() const { return mSceneMgr; }
    Ogre::Camera* getCamera() const { return mCamera; }
    OgreBulletDynamics::DynamicsWorld* getDynamicsWorld() const { return mDynamicsWorld; }
    Player* getPlayer() const { return mPlayer; }
    Level* getLevel() const { return mLevel; }
    
    // Création d'objets physiques
    OgreBulletDynamics::RigidBody* createRigidBody(const Ogre::String& name, 
                                                  float mass, 
                                                  const Ogre::Vector3& pos, 
                                                  const Ogre::Quaternion& quat, 
                                                  OgreBulletCollisions::CollisionShape* shape);
};

#endif // GAME_MANAGER_H