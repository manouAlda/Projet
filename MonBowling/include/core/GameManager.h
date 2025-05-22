#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "AimingSystem.h"
#include "../states/ScoreManager.h"
#include "../utils/PinDetector.h"
#include "../managers/CameraFollower.h"
#include "../include/objects/BowlingBall.h"
#include "../include/objects/BowlingLane.h"
#include <OgreInput.h>
#include <iostream>

// Pattern State pour les états du jeu
enum class GameState {
    AIMING,     // Visée en cours
    POWER,      // Sélection de la puissance
    ROLLING,    // Boule en mouvement
    SCORING,    // Calcul du score
    RESET       // Réinitialisation
};

// Classe principale pour la gestion du jeu (pattern Singleton)
class GameManager {

    private:
        // Constructeur privé (pattern Singleton)
        GameManager();
        
        // Destructeur
        ~GameManager();
        
        // Instance unique
        static GameManager* mInstance;
        
        // État actuel du jeu
        GameState mGameState;
        
        // Références aux objets du jeu
        Ogre::SceneManager* mSceneMgr;
        Ogre::Camera* mCamera;
        BowlingBall* mBall;
        BowlingLane* mLane;
        
        // Systèmes de jeu
        std::unique_ptr<AimingSystem> aiming;
        std::unique_ptr<PinDetector> mPinDetector;
        std::unique_ptr<CameraFollower> mCameraFollower;
        
        // Minuteur pour les transitions d'état
        Ogre::Timer mStateTimer;
        
        // Méthodes de transition d'état
        void changeState(GameState newState);
        void handleAimingState(float deltaTime);
        void handlePowerState(float deltaTime);
        void handleRollingState(float deltaTime);
        void handleScoringState(float deltaTime);
        void handleResetState(float deltaTime);

    public:
        // Obtenir l'instance unique
        static GameManager* getInstance();
        
        // Initialisation du gestionnaire de jeu
        void initialize(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, 
                        BowlingBall* ball, BowlingLane* lane);
        
        // Mise à jour du jeu
        void update(float deltaTime);
        
        // Gestion des événements souris
        bool handleMouseMove(const OgreBites::MouseMotionEvent& evt);
        bool handleMousePress(const OgreBites::MouseButtonEvent& evt);
        bool handleMouseRelease(const OgreBites::MouseButtonEvent& evt);
        
        // Gestion des événements clavier
        bool handleKeyPress(const OgreBites::KeyboardEvent& evt);
        
        // Lancement de la boule
        void launchBall();
        
        // Réinitialisation du jeu
        void resetGame();
        
        // Obtenir l'état actuel du jeu
        GameState getGameState() const;

};

#endif
