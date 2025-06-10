#include "../../include/core/GameManager.h"
#include "../../include/managers/AudioManager.h" 
#include "../../include/states/ScoreManager.h" 
#include <OgreLogManager.h>
#include <OgreStringConverter.h>

// Constantes
const int MAX_FRAMES = 10;

// Initialisation du Singleton
GameManager* GameManager::instance = nullptr;

GameManager* GameManager::getInstance() {
    if (instance == nullptr) {
        instance = new GameManager();
    }
    return instance;
}

GameManager::GameManager()
    : gameState(GameState::AIMING),
      sceneMgr(nullptr),
      camera(nullptr),
      ball(nullptr),
      lane(nullptr),
      currentFrame(1),
      currentRollInFrame(1),
      pinsKnockedFirstRoll(0)
{
}

GameManager::~GameManager() {}
    // Les unique_ptr gèrent leur propre mémoire

void GameManager::initialize(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera,
                            BowlingBall* ball, BowlingLane* lane) {

    this->sceneMgr = sceneMgr; // Correct : assigne le paramètre au membre sceneMgr
    this->camera = camera;     // Correct : assigne le paramètre au membre camera
    this->ball = ball;         // Correct : assigne le paramètre au membre ball
    this->lane = lane;         // Correct : assigne le paramètre au membre lane

    // Initialisation des systèmes qui dépendent de ces pointeurs
    aimingSystem = std::make_unique<AimingSystem>(this->sceneMgr, this->camera);
    aimingSystem->initialize();

    pinDetector = std::make_unique<PinDetector>();
    if (this->lane) { // Utiliser this->lane pour la vérification
        pinDetector->initialize(this->lane->getPins());
    } else {
         Ogre::LogManager::getSingleton().logError("ERREUR: BowlingLane non initialisé avant PinDetector");
    }

    cameraFollower = std::make_unique<CameraFollower>(this->camera, this->ball);
    cameraFollower->initialize();

    ScoreManager::getInstance()->initialize();  // Charger les sons
    AudioManager* audioMgr = AudioManager::getInstance();
    if (!audioMgr->loadSound("bowling-roll/bowling_roll.ogg", "roll", true)) { // Charger en boucle
        Ogre::LogManager::getSingleton().logWarning("Impossible de charger le son de roulement.");
    }
    if (!audioMgr->loadSound("bowling-strike/strike1.wav", "collision", false)) { // Pas en boucle
        Ogre::LogManager::getSingleton().logWarning("Impossible de charger le son de collision.");
    }

    resetGame();

    Ogre::LogManager::getSingleton().logMessage("GameManager initialisé pour un nouveau jeu.");
}

void GameManager::update(float deltaTime) {
    // Mise à jour des systèmes principaux
    if (ball) { 
        ball->update(deltaTime);
    }
    if (lane) { 
        lane->update(deltaTime);
    }
    if (aimingSystem) {
        aimingSystem->update(deltaTime);
    }
    if (pinDetector) {
        pinDetector->update(deltaTime);
    }
    if (cameraFollower) {
        cameraFollower->update(deltaTime);
    }

    // Logique des états de jeu
    switch (gameState) {
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
    if (gameState == GameState::ROLLING && pinDetector /*&& pinDetector->hasDetectedCollisionThisFrame()*/) { // Méthode hypothétique
         AudioManager::getInstance()->playSound("collision");
         //Ogre::LogManager::getSingleton().logMessage("GameManager: Son de collision déclenché (simulation).");
    }
}

// --- Gestion des entrées --- 

bool GameManager::handleMouseMove(const OgreBites::MouseMotionEvent& evt) {
    // La visée par souris n'est plus utilisée par défaut
    // if (gameState == GameState::AIMING && aimingSystem) {
    //     return aimingSystem->handleMouseMove(evt);
    // }
    return false;
}

bool GameManager::handleMousePress(const OgreBites::MouseButtonEvent& evt) {
    // Le clic souris n'est plus utilisé pour passer à POWER ou lancer
    // if (evt.button == OgreBites::BUTTON_LEFT) {
    //     if (gameState == GameState::AIMING && aimingSystem) {
    //         changeState(GameState::POWER);
    //         return true;
    //     } else if (gameState == GameState::POWER && aimingSystem) {
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
    if (gameState == GameState::AIMING && evt.keysym.sym == OgreBites::SDLK_SPACE) {
        changeState(GameState::POWER);
        return true;
    }

    // Transmettre les autres touches à AimingSystem si on est en état POWER
    if (gameState == GameState::POWER && aimingSystem) {
        aimingSystem->handleKeyPress(evt);
        // Note: handleKeyPress ne retourne pas de bool ici, on suppose qu'il gère l'événement
        return true; // Indique que l'événement a été potentiellement géré
    }

    return false;
}

// Nouvelle fonction pour gérer le relâchement de touche
bool GameManager::handleKeyRelease(const OgreBites::KeyboardEvent& evt) {
    // Si on est en état POWER et qu'on relâche HAUT (ou Z/W)
    if (gameState == GameState::POWER &&
        (evt.keysym.sym == OgreBites::SDLK_UP || evt.keysym.sym == 'z' || evt.keysym.sym == 'w'))
    {
        if (aimingSystem) {
            // Indiquer à AimingSystem de gérer le relâchement (il mettra mPowerInputActive à false)
            aimingSystem->handleKeyRelease(evt);
            // Lancer la boule
            launchBall();
            return true;
        }
    }
    return false;
}

// --- Logique des états --- 

void GameManager::changeState(GameState newState) {
    GameState oldState = gameState;
    if (oldState == newState) return;

    gameState = newState;
    Ogre::LogManager::getSingleton().logMessage("Changement d'état : " +
                                               gameStateToString(oldState) +
                                               " -> " +
                                               gameStateToString(newState));

    // Actions à l'entrée du nouvel état
    switch (newState) {
        case GameState::AIMING:
            // Arrêter le son de roulement s'il jouait encore
            AudioManager::getInstance()->stopSound("roll");
            if (aimingSystem) {
                aimingSystem->setAimingActive(true);
                aimingSystem->resetAiming(); // Réinitialise puissance/spin et cache overlays
            }
            
            if (cameraFollower) {
                if(!cameraFollower->isSequenceActive()) {
                   cameraFollower->resetToStartPosition();
                }
            }

            if (ball) {
                ball->reset();
            }
            
            // Réinitialiser les quilles seulement si on commence une nouvelle frame (pas entre 2 lancers)
            if (currentRollInFrame == 1 && lane) {
                 lane->resetPins();
            }
            if (pinDetector) {
                pinDetector->reset(); // Réinitialiser le détecteur pour la nouvelle visée
            }
            break;

        case GameState::POWER:
            if (aimingSystem) {
                aimingSystem->setAimingActive(false); // Désactiver la flèche de visée
                aimingSystem->startPowerPhase(); // Activer l'input puissance/spin et afficher overlays
            }
            break;

        case GameState::ROLLING:
            // Le son de roulement est joué dans launchBall()
            if (aimingSystem) {
                // Cacher les overlays de puissance/spin (déjà fait dans resetAiming lors du passage à AIMING)
                // Mais on peut le faire ici aussi par sécurité si on vient d'un autre état
                aimingSystem->resetAiming(); // Assure que les overlays sont cachés
                aimingSystem->setAimingActive(false);
            }
            if (cameraFollower) {
                cameraFollower->startFollowing();
            }
            if (pinDetector) {
                pinDetector->startDetection(); // Commencer à détecter les quilles tombées
            }
            break;

        case GameState::SCORING:
            // Arrêter le son de roulement
            AudioManager::getInstance()->stopSound("roll");
            // La séquence caméra post-lancer est gérée dans CameraFollower::update
            // quand ball->isRolling() devient false.
            // On pourrait forcer l'arrêt du suivi ici si nécessaire :
            // if (cameraFollower) {
            //     cameraFollower->stopFollowing();
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
    if (ball && !ball->isRolling()) {
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
    if (pinDetector /*&& pinDetector->isDetectionComplete()*/) { // Supposons détection immédiate

        int totalPinsDownSinceReset = pinDetector->getKnockedDownPinCount();
        int pinsThisRoll = (currentRollInFrame == 1) ? totalPinsDownSinceReset : (totalPinsDownSinceReset - pinsKnockedFirstRoll);

        // Jouer le son de collision si des quilles sont tombées
        // Note: Idéalement, ce son serait joué au moment de l'impact.
        // if (pinsThisRoll > 0 && !AudioManager::getInstance()->isPlaying("collision")) { // Eviter répétition rapide
        //     AudioManager::getInstance()->playSound("collision");
        // }

        Ogre::LogManager::getSingleton().logMessage("SCORING: Frame " + Ogre::StringConverter::toString(currentFrame) +
                                                   ", Lancer " + Ogre::StringConverter::toString(currentRollInFrame) +
                                                   ". Quilles ce lancer: " + Ogre::StringConverter::toString(pinsThisRoll));

        // Enregistrer le lancer
        ScoreManager::getInstance()->recordRoll(pinsThisRoll);

        // --- Logique de progression des frames --- 
        bool frameOver = false;
        bool strike = (currentRollInFrame == 1 && pinsThisRoll == 10);
        bool spare = (currentRollInFrame == 2 && (pinsThisRoll + pinsKnockedFirstRoll == 10));

        // Cas spécial : 10ème frame
        if (currentFrame == MAX_FRAMES) {
            if (currentRollInFrame == 1) {
                if (strike) { // Strike au 1er lancer de la 10e
                    //pinsKnockedFirstRoll = 10;
                    pinsKnockedFirstRoll = totalPinsDownSinceReset;
                    currentRollInFrame = 2; // Passe au 2e lancer
                    if(lane) lane->resetPins(); // Reset pour le 2e lancer
                    changeState(GameState::AIMING);
                } else { // Frame ouverte au 1er lancer de la 10e
                    pinsKnockedFirstRoll = pinsThisRoll;
                    currentRollInFrame = 2; // Passe au 2e lancer
                    // Pas de reset des quilles
                    changeState(GameState::AIMING);
                }
            } else if (currentRollInFrame == 2) {
                if (strike || spare) { // Strike ou Spare au 2e lancer -> 3e lancer
                    pinsKnockedFirstRoll += pinsThisRoll; // Cumul pour le 3e
                    currentRollInFrame = 3; // Passe au 3e lancer
                    if(lane) lane->resetPins(); // Reset pour le 3e lancer
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
            } else if (currentRollInFrame == 2) { // Après le 2ème lancer
                if (spare) {
                    Ogre::LogManager::getSingleton().logMessage("Spare!");
                } else {
                    Ogre::LogManager::getSingleton().logMessage("Frame ouverte.");
                }
                frameOver = true;
            } else { // Après le 1er lancer (pas de strike)
                pinsKnockedFirstRoll = pinsThisRoll;
                currentRollInFrame = 2;
                // Pas de reset des quilles
                changeState(GameState::AIMING);
            }
        }

        // Si la frame est terminée (ou la partie)
        if (frameOver) {
            if (currentFrame >= MAX_FRAMES) {
                changeState(GameState::GAME_OVER);
            } else {
                currentFrame++;
                currentRollInFrame = 1;
                pinsKnockedFirstRoll = 0;
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
    if (gameState != GameState::POWER || !aimingSystem || !ball) {
        Ogre::LogManager::getSingleton().logWarning("Tentative de lancement dans un état invalide ou systèmes non prêts.");
        return;
    }

    Ogre::Vector3 direction = aimingSystem->getAimingDirection().normalisedCopy(); // Normalisation
    float power = aimingSystem->getPower();
    float spin = aimingSystem->getSpinEffect();

    Ogre::LogManager::getSingleton().logMessage("Lancement: Dir=" + Ogre::StringConverter::toString(direction) +
                                               ", Power=" + Ogre::StringConverter::toString(power) +
                                               ", Spin=" + Ogre::StringConverter::toString(spin));

    ball->launch(direction, power, spin);

    AudioManager::getInstance()->playSound("roll");
    changeState(GameState::ROLLING);

    Ogre::LogManager::getSingleton().logMessage("Frame " + Ogre::StringConverter::toString(currentFrame) +
                                               ", Lancer " + Ogre::StringConverter::toString(currentRollInFrame) +
                                               ": Boule lancée.");
}

void GameManager::resetGame() {
    Ogre::LogManager::getSingleton().logMessage("Réinitialisation complète du jeu.");
    currentFrame = 1;
    currentRollInFrame = 1;
    pinsKnockedFirstRoll = 0;

    // Arrêter les sons
    AudioManager::getInstance()->stopSound("roll");
    AudioManager::getInstance()->stopSound("collision"); // Au cas où

    // Réinitialiser les systèmes
    if (ball) { ball->reset(); }
    if (lane) { lane->resetPins(); } // S'assurer que les quilles sont debout
    if (aimingSystem) { aimingSystem->resetAiming(); }
    if (pinDetector) { pinDetector->reset(); }
    if (cameraFollower) { cameraFollower->resetToStartPosition(); } // Utiliser resetToStartPosition

    ScoreManager::getInstance()->resetScore();

    // Revenir à l'état de visée initial
    changeState(GameState::AIMING);
}

GameState GameManager::getGameState() const {
    return gameState;
}

CameraFollower* GameManager::getCameraFollower() const {
    return cameraFollower.get();
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

