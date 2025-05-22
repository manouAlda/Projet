#include "../../include/core/GameManager.h"
#include <OgreLogManager.h>
#include <OgreStringConverter.h>

// Initialisation de l'instance statique à nullptr
GameManager* GameManager::mInstance = nullptr;

GameManager* GameManager::getInstance() {
    // Création de l'instance si elle n'existe pas encore
    if (mInstance == nullptr) {
        mInstance = new GameManager();
    }
    return mInstance;
}

GameManager::GameManager()
    : mGameState(GameState::AIMING),
      mSceneMgr(nullptr),
      mCamera(nullptr),
      mBall(nullptr),
      mLane(nullptr) {
}

GameManager::~GameManager() {}

void GameManager::initialize(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, 
                            BowlingBall* ball, BowlingLane* lane) {
    // Stockage des références
    mSceneMgr = sceneMgr;
    mCamera = camera;
    mBall = ball;
    mLane = lane;
    
    // Initialisation des systèmes
    aiming = std::make_unique<AimingSystem>(sceneMgr, camera);
    aiming->initialize();
    aiming->debugForceBar();
    
    mPinDetector = std::make_unique<PinDetector>();
    //mPinDetector->initialize(lane->getPins());
/*    
    mCameraFollower = std::make_unique<CameraFollower>(camera, ball);
    mCameraFollower->initialize();
*/    
    // Initialisation du gestionnaire de score
    //ScoreManager::getInstance()->initialize();
    
    // État initial
    changeState(GameState::AIMING);
    
    Ogre::LogManager::getSingleton().logMessage("GameManager initialisé");
}

void GameManager::update(float deltaTime) {
    // Mise à jour en fonction de l'état actuel
    switch (mGameState) {
        case GameState::AIMING:
            handleAimingState(deltaTime);
            break;
        case GameState::POWER:
            handlePowerState(deltaTime);
            break;
        case GameState::ROLLING:
            handleRollingState(deltaTime);
            break;
        case GameState::SCORING:
            handleScoringState(deltaTime);
            break;
        case GameState::RESET:
            handleResetState(deltaTime);
            break;
    }
    
    // Mise à jour des systèmes
    if (aiming) {
        aiming->update(deltaTime);
    }
   
    if (mPinDetector) {
        mPinDetector->update(deltaTime);
    }
 /*
    if (mCameraFollower) {
        mCameraFollower->update(deltaTime);
    }
*/
}

bool GameManager::handleMouseMove(const OgreBites::MouseMotionEvent& evt) {
    // Transmission de l'événement au système de visée si actif
    if (mGameState == GameState::AIMING && aiming) {
        aiming->handleMouseMove(evt);
        return true;
    }
    return false;
}

bool GameManager::handleMousePress(const OgreBites::MouseButtonEvent& evt) {
    // Gestion du clic de souris en fonction de l'état
    if (mGameState == GameState::AIMING && aiming) {
        // En mode visée, le clic peut démarrer la barre de puissance
        if (evt.button == OgreBites::BUTTON_LEFT) {
            // Vérification si le clic est sur le bouton d'annulation
            if (evt.x >= 20 && evt.x <= 120 && evt.y >= 320 && evt.y <= 360) {
                resetGame();
                return true;
            }
            // Sinon, passage à l'état de puissance
            changeState(GameState::POWER);
            return true;
        }
    } else if (mGameState == GameState::POWER && aiming) {
        // En mode puissance, le clic arrête la barre et lance la boule
        if (evt.button == OgreBites::BUTTON_LEFT) {
            launchBall();
            return true;
        }
    }
    
    return false;
}

bool GameManager::handleMouseRelease(const OgreBites::MouseButtonEvent& evt) {
    // Gestion du relâchement de la souris
    return false;
}

bool GameManager::handleKeyPress(const OgreBites::KeyboardEvent& evt) {
    // Gestion des touches clavier
    if (evt.keysym.sym == OgreBites::SDLK_SPACE) {
        // Espace pour lancer la boule en mode puissance
        if (mGameState == GameState::POWER) {
            launchBall();
            return true;
        }
        // Espace pour démarrer la barre de puissance en mode visée
        else if (mGameState == GameState::AIMING) {
            changeState(GameState::POWER);
            return true;
        }
    } else if (evt.keysym.sym == 'r' || evt.keysym.sym == 'R') {
        // R pour réinitialiser
        resetGame();
        return true;
    }
    
    return false;
}

void GameManager::launchBall() {
    if (mGameState != GameState::POWER || !aiming || !mBall) {
        return;
    }
    
    // Récupération des paramètres de lancer
    Ogre::Vector3 direction = aiming->getAimingDirection();
    float power = aiming->getPower();
    float spin = aiming->getSpinEffect();
    aiming->hideAimingArrow();

    aiming->hideAimingArrow();
    aiming->stopPowerBar();
    
    // Lancement de la boule
    mBall->launch(direction, power, spin);
/*    
    // Démarrage du suivi de la caméra
    if (mCameraFollower) {
        mCameraFollower->startFollowing();
    }
*/    
    // Passage à l'état de roulement
    changeState(GameState::ROLLING);
    
    Ogre::LogManager::getSingleton().logMessage("Boule lancée avec direction=" + 
                                               Ogre::StringConverter::toString(direction) + 
                                               ", puissance=" + Ogre::StringConverter::toString(power) + 
                                               ", effet=" + Ogre::StringConverter::toString(spin));
}

void GameManager::resetGame() {
    // Réinitialisation de la boule
    if (mBall) {
        mBall->reset();
    }
    
    // Réinitialisation des quilles
    if (mLane) {
        mLane->resetPins();
    }
    
    // Réinitialisation des systèmes
    if (aiming) {
        aiming->resetAiming();
    }
    
    if (mPinDetector) {
        mPinDetector->reset();
    }
/*   
    if (mCameraFollower) {
        mCameraFollower->stopFollowing();
    }
*/    
    // Passage à l'état de visée
    changeState(GameState::AIMING);
    
    Ogre::LogManager::getSingleton().logMessage("Jeu réinitialisé");
}

GameState GameManager::getGameState() const {
    return mGameState;
}

void GameManager::changeState(GameState newState) {
    // Transition d'état
    GameState oldState = mGameState;
    mGameState = newState;
    
    // Actions spécifiques à la transition
    switch (newState) {
        case GameState::AIMING:
            if (aiming) {
                aiming->setAimingActive(true);
            }
            break;
        case GameState::POWER:
            if (aiming) {
                aiming->startPowerBar();
            }
            break;
        case GameState::ROLLING:
            if (aiming) {
                aiming->setAimingActive(false);
            }
            if (mPinDetector) {
                mPinDetector->startDetection();
            }
            break;
        case GameState::SCORING:
            // Rien de spécial à faire ici
            break;
        case GameState::RESET:
            mStateTimer.reset();
            break;
    }
    
    Ogre::LogManager::getSingleton().logMessage("État actuel : " + Ogre::StringConverter::toString(static_cast<int>(mGameState)));
    Ogre::LogManager::getSingleton().logMessage("Changement d'état : " + 
                                               Ogre::StringConverter::toString(static_cast<int>(oldState)) + 
                                               " -> " + 
                                               Ogre::StringConverter::toString(static_cast<int>(newState)));
}

void GameManager::handleAimingState(float deltaTime) {
    aiming->showAimingArrow();
    aiming->setAimingActive(true);
}

void GameManager::handlePowerState(float deltaTime) {
    aiming->startPowerBar();
}

void GameManager::handleRollingState(float deltaTime) {
    aiming->hidePowerBar();
    // Vérification si la boule s'est arrêtée
    if (mBall && !mBall->isRolling()) {
        // Passage à l'état de calcul du score
        changeState(GameState::SCORING);
    }
}

void GameManager::handleScoringState(float deltaTime) {
    // Vérification si la détection des quilles est terminée
    if (mPinDetector && mPinDetector->isDetectionComplete()) {
        // Mise à jour du score
        int knockedDownPins = mPinDetector->getKnockedDownPinCount();
        ScoreManager::getInstance()->updateScore(knockedDownPins);
        
        // Passage à l'état de réinitialisation
        changeState(GameState::RESET);
    }
}

void GameManager::handleResetState(float deltaTime) {
    aiming->resetAiming();
    if (mStateTimer.getMilliseconds() > 2000) {  // 2 secondes
        // Passage à l'état de visée
        changeState(GameState::AIMING);
    }
}
