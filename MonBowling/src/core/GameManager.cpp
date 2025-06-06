// Fichier : GameManager.cpp
#include "../../include/core/GameManager.h"
#include "../../include/managers/AudioManager.h" // Ajout AudioManager
#include "../../include/states/ScoreManager.h" // Assurez-vous que le chemin est correct
#include <OgreLogManager.h>
#include <OgreStringConverter.h>

// Constantes
const int MAX_FRAMES = 10;

// Initialisation du Singleton
GameManager* GameManager::mInstance = nullptr;

GameManager* GameManager::getInstance() {
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

GameManager::~GameManager() {
    // Les unique_ptr gèrent leur propre mémoire
}

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
         Ogre::LogManager::getSingleton().logError("ERREUR: BowlingLane non initialisé avant PinDetector");
    }

    mCameraFollower = std::make_unique<CameraFollower>(camera, ball);
    mCameraFollower->initialize();

    ScoreManager::getInstance()->initialize();

    // Charger les sons
    AudioManager* audioMgr = AudioManager::getInstance();
    if (!audioMgr->loadSound("bowling_roll.mp3", "roll", true)) { // Charger en boucle
        Ogre::LogManager::getSingleton().logWarning("Impossible de charger le son de roulement.");
    }
    if (!audioMgr->loadSound("bowling_collision.mp3", "collision", false)) { // Pas en boucle
        Ogre::LogManager::getSingleton().logWarning("Impossible de charger le son de collision.");
    }

    resetGame();

    Ogre::LogManager::getSingleton().logMessage("GameManager initialisé pour un nouveau jeu.");
}

void GameManager::update(float deltaTime) {
    // Mise à jour des systèmes principaux
    if (mAimingSystem) {
        mAimingSystem->update(deltaTime);
    }
    if (mPinDetector) {
        mPinDetector->update(deltaTime);
    }
    if (mCameraFollower) {
        mCameraFollower->update(deltaTime);
    }
    // La mise à jour de la boule est gérée par PhysicsManager/BowlingBall::update
    // La mise à jour de FMOD est dans Application::frameRenderingQueued

    // Logique des états de jeu
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
            // Gérer l'état de fin de partie (affichage score final, option rejouer...)
            break;
    }

    // --- Détection de collision pour le son --- 
    // Idéalement, ceci serait géré par PhysicsManager ou PinDetector
    // qui appellerait directement AudioManager::getInstance()->playSound("collision");
    // En attendant, on peut le simuler ici si PinDetector expose une info
    if (mGameState == GameState::ROLLING && mPinDetector /*&& mPinDetector->hasDetectedCollisionThisFrame()*/) { // Méthode hypothétique
         AudioManager::getInstance()->playSound("collision");
         Ogre::LogManager::getSingleton().logMessage("GameManager: Son de collision déclenché (simulation).");
    }
}

// --- Gestion des entrées --- 

bool GameManager::handleMouseMove(const OgreBites::MouseMotionEvent& evt) {
    // La visée par souris n'est plus utilisée par défaut
    // if (mGameState == GameState::AIMING && mAimingSystem) {
    //     return mAimingSystem->handleMouseMove(evt);
    // }
    return false;
}

bool GameManager::handleMousePress(const OgreBites::MouseButtonEvent& evt) {
    // Le clic souris n'est plus utilisé pour passer à POWER ou lancer
    // if (evt.button == OgreBites::BUTTON_LEFT) {
    //     if (mGameState == GameState::AIMING && mAimingSystem) {
    //         changeState(GameState::POWER);
    //         return true;
    //     } else if (mGameState == GameState::POWER && mAimingSystem) {
    //         // Le lancement se fait au relâchement de la touche UP maintenant
    //         // launchBall();
    //         return true;
    //     }
    // }
    return false;
}

bool GameManager::handleMouseRelease(const OgreBites::MouseButtonEvent& evt) {
    return false;
}

bool GameManager::handleKeyPress(const OgreBites::KeyboardEvent& evt) {
    // Touche R pour réinitialiser le jeu complet
    if (evt.keysym.sym == 'r' || evt.keysym.sym == 'R') {
        resetGame();
        return true;
    }

    // Touche Espace pour passer de AIMING à POWER
    if (mGameState == GameState::AIMING && evt.keysym.sym == OgreBites::SDLK_SPACE) {
        changeState(GameState::POWER);
        return true;
    }

    // Transmettre les autres touches à AimingSystem si on est en état POWER
    if (mGameState == GameState::POWER && mAimingSystem) {
        mAimingSystem->handleKeyPress(evt);
        // Note: handleKeyPress ne retourne pas de bool ici, on suppose qu'il gère l'événement
        return true; // Indique que l'événement a été potentiellement géré
    }

    return false;
}

// Nouvelle fonction pour gérer le relâchement de touche
bool GameManager::handleKeyRelease(const OgreBites::KeyboardEvent& evt) {
    // Si on est en état POWER et qu'on relâche HAUT (ou Z/W)
    if (mGameState == GameState::POWER &&
        (evt.keysym.sym == OgreBites::SDLK_UP || evt.keysym.sym == 'z' || evt.keysym.sym == 'w'))
    {
        if (mAimingSystem) {
            // Indiquer à AimingSystem de gérer le relâchement (il mettra mPowerInputActive à false)
            mAimingSystem->handleKeyRelease(evt);
            // Lancer la boule
            launchBall();
            return true;
        }
    }
    return false;
}

// --- Logique des états --- 

void GameManager::changeState(GameState newState) {
    GameState oldState = mGameState;
    if (oldState == newState) return;

    mGameState = newState;
    Ogre::LogManager::getSingleton().logMessage("Changement d'état : " +
                                               gameStateToString(oldState) +
                                               " -> " +
                                               gameStateToString(newState));

    // Actions à l'entrée du nouvel état
    switch (newState) {
        case GameState::AIMING:
            // Arrêter le son de roulement s'il jouait encore
            AudioManager::getInstance()->stopSound("roll");
            if (mAimingSystem) {
                mAimingSystem->setAimingActive(true);
                mAimingSystem->resetAiming(); // Réinitialise puissance/spin et cache overlays
            }
            if (mCameraFollower) {
                 mCameraFollower->resetToStartPosition();
            }
            if (mBall) {
                mBall->reset();
            }
            // Réinitialiser les quilles seulement si on commence une nouvelle frame (pas entre 2 lancers)
            if (mCurrentRollInFrame == 1 && mLane) {
                 mLane->resetPins();
            }
            if (mPinDetector) {
                mPinDetector->reset(); // Réinitialiser le détecteur pour la nouvelle visée
            }
            break;

        case GameState::POWER:
            if (mAimingSystem) {
                mAimingSystem->setAimingActive(false); // Désactiver la flèche de visée
                mAimingSystem->startPowerPhase(); // Activer l'input puissance/spin et afficher overlays
            }
            break;

        case GameState::ROLLING:
            // Le son de roulement est joué dans launchBall()
            if (mAimingSystem) {
                // Cacher les overlays de puissance/spin (déjà fait dans resetAiming lors du passage à AIMING)
                // Mais on peut le faire ici aussi par sécurité si on vient d'un autre état
                mAimingSystem->resetAiming(); // Assure que les overlays sont cachés
                mAimingSystem->setAimingActive(false);
            }
            if (mCameraFollower) {
                mCameraFollower->startFollowing();
            }
            if (mPinDetector) {
                mPinDetector->startDetection(); // Commencer à détecter les quilles tombées
            }
            break;

        case GameState::SCORING:
            // Arrêter le son de roulement
            AudioManager::getInstance()->stopSound("roll");
            // La séquence caméra post-lancer est gérée dans CameraFollower::update
            // quand mBall->isRolling() devient false.
            // On pourrait forcer l'arrêt du suivi ici si nécessaire :
            // if (mCameraFollower) {
            //     mCameraFollower->stopFollowing();
            // }
            break;

        case GameState::GAME_OVER:
             AudioManager::getInstance()->stopSound("roll"); // Sécurité
             Ogre::LogManager::getSingleton().logMessage("Partie terminée! Score final: " + Ogre::StringConverter::toString(ScoreManager::getInstance()->getCurrentScore()));
             // Afficher un message à l'utilisateur, proposer de rejouer (touche R?)
            break;
    }
}

void GameManager::handleAimingState(float deltaTime) {
    // La direction est gérée par la souris (si activé) ou fixe
    // Le passage à POWER se fait via la touche Espace (handleKeyPress)
}

void GameManager::handlePowerState(float deltaTime) {
    // La puissance/spin sont gérés par AimingSystem via handleKeyPress
    // Le lancement se fait via handleKeyRelease
}

void GameManager::handleRollingState(float deltaTime) {
    // Vérification si la boule a fini de rouler
    if (mBall && !mBall->isRolling()) {
        Ogre::LogManager::getSingleton().logMessage("Boule arrêtée. Passage à SCORING.");
        changeState(GameState::SCORING);
    }
    // Mettre à jour le son de roulement (volume basé sur vitesse?)
    // Pour l'instant, il joue en boucle jusqu'à SCORING ou AIMING.
}

void GameManager::handleScoringState(float deltaTime) {
    // Attendre que la séquence caméra post-lancer soit terminée ?
    // Ou calculer le score dès que la boule s'arrête.
    // Pour l'instant, calcul immédiat.

    // S'assurer que le détecteur a fini son travail (si asynchrone)
    if (mPinDetector /*&& mPinDetector->isDetectionComplete()*/) { // Supposons détection immédiate

        int totalPinsDownSinceReset = mPinDetector->getKnockedDownPinCount();
        int pinsThisRoll = totalPinsDownSinceReset - mPinsKnockedFirstRoll;

        // Jouer le son de collision si des quilles sont tombées
        // Note: Idéalement, ce son serait joué au moment de l'impact.
        // if (pinsThisRoll > 0 && !AudioManager::getInstance()->isPlaying("collision")) { // Eviter répétition rapide
        //     AudioManager::getInstance()->playSound("collision");
        // }

        Ogre::LogManager::getSingleton().logMessage("SCORING: Frame " + Ogre::StringConverter::toString(mCurrentFrame) +
                                                   ", Lancer " + Ogre::StringConverter::toString(mCurrentRollInFrame) +
                                                   ". Quilles ce lancer: " + Ogre::StringConverter::toString(pinsThisRoll));

        // Enregistrer le lancer
        ScoreManager::getInstance()->recordRoll(pinsThisRoll);

        // --- Logique de progression des frames --- 
        bool frameOver = false;
        bool strike = (mCurrentRollInFrame == 1 && pinsThisRoll == 10);
        bool spare = (mCurrentRollInFrame == 2 && (pinsThisRoll + mPinsKnockedFirstRoll == 10));

        // Cas spécial : 10ème frame
        if (mCurrentFrame == MAX_FRAMES) {
            if (mCurrentRollInFrame == 1) {
                if (strike) { // Strike au 1er lancer de la 10e
                    mPinsKnockedFirstRoll = 10;
                    mCurrentRollInFrame = 2; // Passe au 2e lancer
                    if(mLane) mLane->resetPins(); // Reset pour le 2e lancer
                    changeState(GameState::AIMING);
                } else { // Frame ouverte au 1er lancer de la 10e
                    mPinsKnockedFirstRoll = pinsThisRoll;
                    mCurrentRollInFrame = 2; // Passe au 2e lancer
                    // Pas de reset des quilles
                    changeState(GameState::AIMING);
                }
            } else if (mCurrentRollInFrame == 2) {
                if (strike || spare) { // Strike ou Spare au 2e lancer -> 3e lancer
                    mPinsKnockedFirstRoll += pinsThisRoll; // Cumul pour le 3e
                    mCurrentRollInFrame = 3; // Passe au 3e lancer
                    if(mLane) mLane->resetPins(); // Reset pour le 3e lancer
                    changeState(GameState::AIMING);
                } else { // Frame ouverte après 2 lancers -> Fin de partie
                    frameOver = true;
                }
            } else { // Après le 3ème lancer -> Fin de partie
                frameOver = true;
            }
        } 
        // Cas : Frames 1 à 9
        else {
            if (strike) { // Strike
                Ogre::LogManager::getSingleton().logMessage("Strike!");
                frameOver = true;
            } else if (mCurrentRollInFrame == 2) { // Après le 2ème lancer
                if (spare) {
                    Ogre::LogManager::getSingleton().logMessage("Spare!");
                } else {
                    Ogre::LogManager::getSingleton().logMessage("Frame ouverte.");
                }
                frameOver = true;
            } else { // Après le 1er lancer (pas de strike)
                mPinsKnockedFirstRoll = pinsThisRoll;
                mCurrentRollInFrame = 2;
                // Pas de reset des quilles
                changeState(GameState::AIMING);
            }
        }

        // Si la frame est terminée (ou la partie)
        if (frameOver) {
            if (mCurrentFrame >= MAX_FRAMES) {
                changeState(GameState::GAME_OVER);
            } else {
                mCurrentFrame++;
                mCurrentRollInFrame = 1;
                mPinsKnockedFirstRoll = 0;
                // Le reset des quilles se fait à l'entrée de AIMING
                changeState(GameState::AIMING);
            }
        }

    } else {
        Ogre::LogManager::getSingleton().logWarning("Attente de PinDetector ou PinDetector non initialisé.");
        // Peut-être ajouter un timeout pour éviter de rester bloqué ici
    }
}

// --- Autres méthodes --- 

void GameManager::launchBall() {
    if (mGameState != GameState::POWER || !mAimingSystem || !mBall) {
        Ogre::LogManager::getSingleton().logWarning("Tentative de lancement dans un état invalide ou systèmes non prêts.");
        return;
    }

    Ogre::Vector3 direction = mAimingSystem->getAimingDirection();
    float power = mAimingSystem->getPower(); // Converti en vitesse dans AimingSystem
    float spin = mAimingSystem->getSpinEffect(); // Converti en vitesse angulaire dans AimingSystem

    Ogre::LogManager::getSingleton().logMessage("Lancement: Dir=" + Ogre::StringConverter::toString(direction) +
                                               ", Power=" + Ogre::StringConverter::toString(power) +
                                               ", Spin=" + Ogre::StringConverter::toString(spin));

    mBall->launch(direction, power, spin);

    // Jouer le son de roulement
    AudioManager::getInstance()->playSound("roll");

    // Passer à l'état ROLLING (qui gère la caméra, etc.)
    changeState(GameState::ROLLING);

    Ogre::LogManager::getSingleton().logMessage("Frame " + Ogre::StringConverter::toString(mCurrentFrame) +
                                               ", Lancer " + Ogre::StringConverter::toString(mCurrentRollInFrame) +
                                               ": Boule lancée.");
}

void GameManager::resetGame() {
    Ogre::LogManager::getSingleton().logMessage("Réinitialisation complète du jeu.");
    mCurrentFrame = 1;
    mCurrentRollInFrame = 1;
    mPinsKnockedFirstRoll = 0;

    // Arrêter les sons
    AudioManager::getInstance()->stopSound("roll");
    AudioManager::getInstance()->stopSound("collision"); // Au cas où

    // Réinitialiser les systèmes
    if (mBall) { mBall->reset(); }
    if (mLane) { mLane->resetPins(); } // S'assurer que les quilles sont debout
    if (mAimingSystem) { mAimingSystem->resetAiming(); }
    if (mPinDetector) { mPinDetector->reset(); }
    if (mCameraFollower) { mCameraFollower->resetToStartPosition(); } // Utiliser resetToStartPosition

    ScoreManager::getInstance()->resetScore();

    // Revenir à l'état de visée initial
    changeState(GameState::AIMING);
}

GameState GameManager::getGameState() const {
    return mGameState;
}

CameraFollower* GameManager::getCameraFollower() const {
    return mCameraFollower.get();
}

std::string GameManager::gameStateToString(GameState state) {
    switch (state) {
        case GameState::AIMING: return "AIMING";
        case GameState::POWER: return "POWER";
        case GameState::ROLLING: return "ROLLING";
        case GameState::SCORING: return "SCORING";
        case GameState::GAME_OVER: return "GAME_OVER";
        default: return "UNKNOWN";
    }
}

