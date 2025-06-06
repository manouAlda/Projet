#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <Ogre.h>
#include <memory> // Pour std::unique_ptr
#include <string> // Pour std::string

#include "AimingSystem.h"
#include "../states/ScoreManager.h"
#include "../utils/PinDetector.h"
#include "../managers/CameraFollower.h"
#include "../include/objects/BowlingBall.h"
#include "../include/objects/BowlingLane.h"

// Forward declarations
namespace Ogre {
    class SceneManager;
    class Camera;
    class Quaternion;
}
namespace OgreBites {
    class MouseMotionEvent;
    class MouseButtonEvent;
    class KeyboardEvent;
}
class BowlingBall;
class BowlingLane;
class AimingSystem;
class PinDetector;
class CameraFollower;
class ScoreManager; // Forward declare ScoreManager

// Enum pour les états de jeu
enum class GameState {
    AIMING,
    POWER,
    ROLLING,
    SCORING,
    // RESET, // Obsolète, géré par AIMING/resetGame
    GAME_OVER
};

class GameManager {
public:
    // Méthode statique pour obtenir l'instance (Singleton)
    static GameManager* getInstance();

    // Initialisation
    void initialize(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, BowlingBall* ball, BowlingLane* lane);

    // Mise à jour principale
    void update(float deltaTime);

    // Réinitialisation complète du jeu
    void resetGame();

    // Lancement de la boule (appelé depuis handleKeyRelease)
    void launchBall();

    // Obtient l'état actuel du jeu
    GameState getGameState() const;

    // Change l'état du jeu
    void changeState(GameState newState);

    // --- Gestionnaires d'événements d'entrée --- 
    bool handleMouseMove(const OgreBites::MouseMotionEvent& evt);
    bool handleMousePress(const OgreBites::MouseButtonEvent& evt);
    bool handleMouseRelease(const OgreBites::MouseButtonEvent& evt);
    bool handleKeyPress(const OgreBites::KeyboardEvent& evt);
    // Ajout de la déclaration manquante pour handleKeyRelease
    bool handleKeyRelease(const OgreBites::KeyboardEvent& evt);

    // Accesseur pour CameraFollower (si nécessaire depuis l'extérieur)
    CameraFollower* getCameraFollower() const;

private:
    // Constructeur/Destructeur privés (Singleton)
    GameManager();
    ~GameManager();

    // Empêcher la copie (Singleton)
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // Instance unique
    static GameManager* mInstance;

    // --- Membres --- 
    GameState mGameState;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
    BowlingBall* mBall; // Pointeur brut, durée de vie gérée par Application
    BowlingLane* mLane; // Pointeur brut, durée de vie gérée par Application

    // Systèmes gérés par GameManager
    std::unique_ptr<AimingSystem> mAimingSystem;
    std::unique_ptr<PinDetector> mPinDetector;
    std::unique_ptr<CameraFollower> mCameraFollower;
    // ScoreManager est aussi un Singleton, pas besoin de le stocker ici

    // Variables de suivi du jeu
    int mCurrentFrame;
    int mCurrentRollInFrame;
    int mPinsKnockedFirstRoll;
    const int MAX_FRAMES = 10; // Déplacé ici depuis GameManager.cpp

    // --- Méthodes privées pour la logique des états --- 
    void handleAimingState(float deltaTime);
    void handlePowerState(float deltaTime);
    void handleRollingState(float deltaTime);
    void handleScoringState(float deltaTime);

    // Utilitaire pour convertir l'état en chaîne (pour les logs)
    std::string gameStateToString(GameState state);
};

#endif 

