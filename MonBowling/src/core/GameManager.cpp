#include "../../include/core/GameManager.h"
#include "../../include/states/ScoreManager.h" 
#include <OgreLogManager.h>
#include <OgreStringConverter.h>

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
      mLane(nullptr),
      mCurrentFrame(1),
      mCurrentRollInFrame(1),
      mPinsKnockedFirstRoll(0) 
{
}

GameManager::~GameManager() {}

void GameManager::initialize(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, 
                            BowlingBall* ball, BowlingLane* lane) {
    
    mSceneMgr = sceneMgr;
    mCamera = camera;
    mBall = ball;
    mLane = lane;
    
    // Initialisation des systèmes
    mAimingSystem = std::make_unique<AimingSystem>(sceneMgr, camera);
    mAimingSystem->initialize();
    
    mPinDetector = std::make_unique<PinDetector>();
   
    if (mLane) {
        mPinDetector->initialize(mLane->getPins()); 
    } else {
         Ogre::LogManager::getSingleton().logMessage("ERREUR: BowlingLane non initialisé avant PinDetector");
    }
    
    mCameraFollower = std::make_unique<CameraFollower>(camera, ball);
    mCameraFollower->initialize();
    
    ScoreManager::getInstance()->initialize();
    
    resetGame(); 
    
    Ogre::LogManager::getSingleton().logMessage("GameManager initialisé pour un nouveau jeu.");
}

void GameManager::update(float deltaTime) {
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
        case GameState::GAME_OVER: 
            break;
    }
    
    if (mGameState != GameState::GAME_OVER) {
        if (mAimingSystem) {
            mAimingSystem->update(deltaTime);
        }
        if (mPinDetector) {
            mPinDetector->update(deltaTime); 
        }
        if (mCameraFollower) {
            mCameraFollower->update(deltaTime);
        }
    }
}

bool GameManager::handleMouseMove(const OgreBites::MouseMotionEvent& evt) {
    if (mGameState == GameState::AIMING && mAimingSystem) {
        mAimingSystem->handleMouseMove(evt);
        return true;
    }
    return false;
}

bool GameManager::handleMousePress(const OgreBites::MouseButtonEvent& evt) {
    if (evt.button == OgreBites::BUTTON_LEFT) {
        if (mGameState == GameState::AIMING && mAimingSystem) {
            changeState(GameState::POWER);
            return true;
        } else if (mGameState == GameState::POWER && mAimingSystem) {
            launchBall();
            return true;
        }
    }
    return false;
}

bool GameManager::handleMouseRelease(const OgreBites::MouseButtonEvent& evt) {
    return false;
}

bool GameManager::handleKeyPress(const OgreBites::KeyboardEvent& evt) {
    if (evt.keysym.sym == OgreBites::SDLK_SPACE) {
        if (mGameState == GameState::POWER) {
            launchBall();
            return true;
        } else if (mGameState == GameState::AIMING) {
            changeState(GameState::POWER);
            return true;
        }
    } else if (evt.keysym.sym == 'r' || evt.keysym.sym == 'R') {
        // La touche R réinitialise TOUT le jeu
        resetGame();
        return true;
    }
    return false;
}

void GameManager::launchBall() {
    if (mGameState != GameState::POWER || !mAimingSystem || !mBall) {
        return;
    }
    
    Ogre::Vector3 direction = mAimingSystem->getAimingDirection();
    float power = mAimingSystem->getPower(); 
    float spin = mAimingSystem->getSpinEffect();
    //power = 25.0f; 
    
    mBall->launch(direction, power, spin);
    
    if (mCameraFollower) {
        mCameraFollower->startFollowing();
    }
    
    if (mPinDetector) {
        //mPinDetector->reset(); 
        mPinDetector->startDetection(); 
    }

    changeState(GameState::ROLLING);
    
    Ogre::LogManager::getSingleton().logMessage("Frame " + Ogre::StringConverter::toString(mCurrentFrame) + ", Lancer " + Ogre::StringConverter::toString(mCurrentRollInFrame) + ": Boule lancée.");
}

void GameManager::resetGame() {
    Ogre::LogManager::getSingleton().logMessage("Réinitialisation complète du jeu.");
    mCurrentFrame = 1;
    mCurrentRollInFrame = 1;
    mPinsKnockedFirstRoll = 0;

    if (mBall) { mBall->reset(); }
    if (mLane) { mLane->resetPins(); } 
    if (mAimingSystem) { mAimingSystem->resetAiming(); }
    if (mPinDetector) { mPinDetector->reset(); } 
    if (mCameraFollower) { mCameraFollower->stopFollowing(); }
    
    ScoreManager::getInstance()->resetScore(); 
    
    changeState(GameState::AIMING); 
}

GameState GameManager::getGameState() const {
    return mGameState;
}

CameraFollower* GameManager::getCameraFollower() const {
    return mCameraFollower.get();
}

void GameManager::changeState(GameState newState) {
    GameState oldState = mGameState;
    if (oldState == newState) return; 

    mGameState = newState;
    Ogre::LogManager::getSingleton().logMessage("Changement d'état : " + 
                                               gameStateToString(oldState) + 
                                               " -> " + 
                                               gameStateToString(newState));

    switch (newState) {
        case GameState::AIMING:
            if (mAimingSystem) {
                mAimingSystem->setAimingActive(true);
                mAimingSystem->resetAiming(); 
            }
            if (mCameraFollower) {
                 mCameraFollower->resetToStartPosition(); 
            }
            if (mBall) {
                mBall->reset();
            }
            break;
        case GameState::POWER:
            if (mAimingSystem) {
                mAimingSystem->startPowerBar();
            }
            break;
        case GameState::ROLLING:
            if (mAimingSystem) {
                mAimingSystem->setAimingActive(false);
            }
            break;
        case GameState::SCORING:
            if (mCameraFollower) {
                mCameraFollower->stopFollowing(); 
            }
            break;
        case GameState::GAME_OVER:
             Ogre::LogManager::getSingleton().logMessage("Partie terminée! Score final: " + Ogre::StringConverter::toString(ScoreManager::getInstance()->getCurrentScore()));
            break;
    }
}

void GameManager::handleAimingState(float deltaTime) {
    // La visée est gérée par les événements souris/clavier
}

void GameManager::handlePowerState(float deltaTime) {
    // La barre de puissance est mise à jour par AimingSystem
}

void GameManager::handleRollingState(float deltaTime) {
    // Vérification si la boule a fini de rouler
    if (mBall && !mBall->isRolling()) {
        Ogre::LogManager::getSingleton().logMessage("Boule arrêtée. Passage à SCORING.");
        // Potentiellement attendre un court instant pour que la physique se stabilise
        // avant de passer au scoring ? Pour l'instant, transition directe.
        changeState(GameState::SCORING);
    }
}

void GameManager::handleScoringState(float deltaTime) {
    // S'assurer que le détecteur est prêt et que la détection est complète
    // La condition isDetectionComplete() dépend de l'implémentation de PinDetector
    // Elle pourrait signifier "attendre X secondes après l'arrêt de la boule"
    // ou "la physique s'est stabilisée".
    if (mPinDetector /*&& mPinDetector->isDetectionComplete()*/) { // Supposons que la détection est immédiate pour l'instant
        
        int totalPinsDown = mPinDetector->getKnockedDownPinCount();
        int pinsThisRoll = totalPinsDown - mPinsKnockedFirstRoll;
        
        Ogre::LogManager::getSingleton().logMessage("SCORING: Frame " + Ogre::StringConverter::toString(mCurrentFrame) + 
                                                   ", Lancer " + Ogre::StringConverter::toString(mCurrentRollInFrame) + 
                                                   ". Total quilles tombées: " + Ogre::StringConverter::toString(totalPinsDown) + 
                                                   ". Quilles ce lancer: " + Ogre::StringConverter::toString(pinsThisRoll));

        // Enregistrer le lancer dans le ScoreManager
        ScoreManager::getInstance()->recordRoll(pinsThisRoll);

        // Logique de progression des frames (simplifiée pour 1-9)
        // TODO: Gérer la 10ème frame spécifiquement
        if (mCurrentFrame >= MAX_FRAMES) {
             // Logique spéciale 10ème frame ici...
             // Pour l'instant, on termine le jeu après la 10ème frame standard
             if (mCurrentRollInFrame == 2 || pinsThisRoll == 10) { // Fin de la 10e frame
                 changeState(GameState::GAME_OVER);
                 return; // Sortir pour éviter la logique standard
             } else { // Premier lancer de la 10e frame (pas un strike)
                 mPinsKnockedFirstRoll = totalPinsDown;
                 mCurrentRollInFrame = 2;
                 // Ne pas réinitialiser les quilles pour le 2e lancer de la 10e
                 changeState(GameState::AIMING); 
                 return;
             }
        }

        // Logique pour les frames 1 à 9
        if (mCurrentRollInFrame == 1) {
            if (pinsThisRoll == 10) { // Strike!
                Ogre::LogManager::getSingleton().logMessage("Strike!");
                mCurrentFrame++;
                mCurrentRollInFrame = 1;
                mPinsKnockedFirstRoll = 0;
                if (mLane) mLane->resetPins(); // Réinitialiser les quilles pour la nouvelle frame
                changeState(GameState::AIMING);
            } else { // Pas de strike
                mPinsKnockedFirstRoll = totalPinsDown; // Garder le compte pour le 2e lancer
                mCurrentRollInFrame = 2;
                // NE PAS réinitialiser les quilles
                changeState(GameState::AIMING);
            }
        } else { // C'était le 2ème lancer
             if (pinsThisRoll + mPinsKnockedFirstRoll == 10) {
                 Ogre::LogManager::getSingleton().logMessage("Spare!");
             } else {
                 Ogre::LogManager::getSingleton().logMessage("Frame ouverte.");
             }
            mCurrentFrame++;
            mCurrentRollInFrame = 1;
            mPinsKnockedFirstRoll = 0;
            if (mLane) mLane->resetPins(); // Réinitialiser les quilles pour la nouvelle frame
            changeState(GameState::AIMING);
        }

    } else {
        // Attendre que la détection soit complète ?
        // Ou il y a un problème avec PinDetector
        Ogre::LogManager::getSingleton().logMessage("Attente de PinDetector ou PinDetector non initialisé.");
    }
}

// À ajouter dans le .h ou ici si static
std::string GameManager::gameStateToString(GameState state) {
    switch (state) {
        case GameState::AIMING: return "AIMING";
        case GameState::POWER: return "POWER";
        case GameState::ROLLING: return "ROLLING";
        case GameState::SCORING: return "SCORING";
        //case GameState::RESET: return "RESET"; // Obsolète
        case GameState::GAME_OVER: return "GAME_OVER";
        default: return "UNKNOWN";
    }
}

