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
#include <OgreInput.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include <OgreTimer.h>

enum class GameState {
    AIMING,     
    POWER,      
    ROLLING,    
    SCORING,    
    RESET,      
    GAME_OVER 
};

class GameManager {
    public:
        static GameManager* getInstance();
        void initialize(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, BowlingBall* ball, BowlingLane* lane);
        void update(float deltaTime);
        void resetGame();
        void launchBall();
        GameState getGameState() const;
        void changeState(GameState newState);
        bool handleMouseMove(const OgreBites::MouseMotionEvent& evt);
        bool handleMousePress(const OgreBites::MouseButtonEvent& evt);
        bool handleMouseRelease(const OgreBites::MouseButtonEvent& evt);
        bool handleKeyPress(const OgreBites::KeyboardEvent& evt);
    
        CameraFollower* getCameraFollower() const;
        
    private:
        GameManager();
        ~GameManager();
    
        static GameManager* mInstance;
    
        // Membres ajoutés
        int mCurrentFrame;               // Frame actuelle
        int mCurrentRollInFrame;         // Lancer actuel dans la frame
        int mPinsKnockedFirstRoll;       // Quilles tombées au premier lancer
        const int MAX_FRAMES = 10;       // Nombre maximum de frames dans une partie
    
        // Autres membres existants
        GameState mGameState;
        Ogre::SceneManager* mSceneMgr;
        Ogre::Camera* mCamera;
        BowlingBall* mBall;
        BowlingLane* mLane;
        std::unique_ptr<AimingSystem> mAimingSystem;
        std::unique_ptr<PinDetector> mPinDetector;
        std::unique_ptr<CameraFollower> mCameraFollower;
    
        // Méthodes privées
        void handleAimingState(float deltaTime);
        void handlePowerState(float deltaTime);
        void handleRollingState(float deltaTime);
        void handleScoringState(float deltaTime);
        std::string gameStateToString(GameState state); // Ajout de la déclaration
    };

#endif 
