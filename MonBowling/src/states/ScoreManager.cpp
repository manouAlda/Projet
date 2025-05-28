#include "../../include/states/ScoreManager.h"
#include <OgreStringConverter.h>
#include <vector>
#include <numeric>
#include <OgreLogManager.h>

// Initialisation de l'instance statique à nullptr
ScoreManager* ScoreManager::mInstance = nullptr;

ScoreManager* ScoreManager::getInstance() {
    // Création de l'instance si elle n'existe pas encore
    if (mInstance == nullptr) {
        mInstance = new ScoreManager();
    }
    return mInstance;
}

ScoreManager::ScoreManager()
    : mScoreOverlay(nullptr),
      mScoreContainer(nullptr),
      mScoreText(nullptr) {
    resetScore(); // Initialise les scores lors de la création
}

ScoreManager::~ScoreManager() {
    // Nettoyage des overlays
    if (mScoreOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(mScoreOverlay);
    }
}

void ScoreManager::initialize() {
    // Création de l'overlay pour l'affichage du score
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    mScoreOverlay = overlayManager.create("ScoreOverlay");
    
    // Création du conteneur pour l'affichage du score
    mScoreContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "ScoreContainer"));
    mScoreContainer->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreContainer->setPosition(20, 20);  // Position en haut à gauche
    mScoreContainer->setDimensions(200, 30);
    
    // Création du texte pour l'affichage du score
    mScoreText = static_cast<Ogre::TextAreaOverlayElement*>(
        overlayManager.createOverlayElement("TextArea", "ScoreText"));
    mScoreText->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreText->setPosition(0, 0);
    mScoreText->setDimensions(200, 30);
    mScoreText->setCharHeight(24);
    mScoreText->setFontName("Arial"); // Assurez-vous que cette police est disponible
    mScoreText->setColour(Ogre::ColourValue::White);
    mScoreText->setCaption("Score: 0");
    
    // Ajout du texte au conteneur
    mScoreContainer->addChild(mScoreText);
    
    // Ajout du conteneur à l'overlay
    mScoreOverlay->add2D(mScoreContainer);
    
    // Affichage de l'overlay
    mScoreOverlay->setZOrder(100);
    mScoreOverlay->show();
    
    // Réinitialisation du score (déjà fait dans le constructeur, mais bon pour la clarté)
    resetScore();
}

// Enregistre le nombre de quilles abattues pour le lancer actuel
void ScoreManager::recordRoll(int pins) {
    if (mCurrentRoll < MAX_ROLLS) {
        mRolls[mCurrentRoll++] = pins;
        Ogre::LogManager::getSingleton().logMessage("ScoreManager: Roll " + Ogre::StringConverter::toString(mCurrentRoll) + " recorded: " + Ogre::StringConverter::toString(pins) + " pins.");
        calculateScore(); // Recalculer le score total après chaque lancer
        updateScoreDisplay();
    } else {
        Ogre::LogManager::getSingleton().logMessage("ScoreManager: Maximum rolls reached.");
    }
}

// Réinitialise toutes les données de score
void ScoreManager::resetScore() {
    mRolls.fill(0);
    mFrameScores.fill(0);
    mCurrentRoll = 0;
    mTotalScore = 0;
    if (mScoreText) { // Vérifier si l'UI est initialisée
        updateScoreDisplay();
    }
    Ogre::LogManager::getSingleton().logMessage("ScoreManager: Score reset.");
}

// Calcule le score total basé sur les lancers enregistrés
void ScoreManager::calculateScore() {
    mTotalScore = 0;
    mFrameScores.fill(0);
    int rollIndex = 0;
    
    for (int frame = 0; frame < 10; ++frame) {
        if (rollIndex >= mCurrentRoll) break; // Ne pas calculer si les lancers n'ont pas encore eu lieu

        if (isStrike(rollIndex)) { // Strike
            if (rollIndex + 2 < mCurrentRoll) { // Vérifier si les lancers bonus existent
                 mFrameScores[frame] = 10 + strikeBonus(rollIndex);
                 mTotalScore += mFrameScores[frame];
            }
            rollIndex++;
        } else if (isSpare(rollIndex)) { // Spare
            if (rollIndex + 2 < mCurrentRoll) { // Vérifier si le lancer bonus existe
                mFrameScores[frame] = 10 + spareBonus(rollIndex);
                mTotalScore += mFrameScores[frame];
            }
            rollIndex += 2;
        } else { // Open frame
            if (rollIndex + 1 < mCurrentRoll) { // Vérifier si les deux lancers existent
                mFrameScores[frame] = openFrameScore(rollIndex);
                mTotalScore += mFrameScores[frame];
            }
            rollIndex += 2;
        }
        
        // Gérer le cas où le calcul s'arrête car les lancers suivants n'ont pas été faits
        if (rollIndex >= mCurrentRoll && frame < 9) {
             // Si on ne peut pas calculer le score complet de la frame (manque de lancers bonus),
             // on arrête le calcul cumulatif ici pour ne pas afficher un score erroné.
             // On pourrait afficher un score partiel ou attendre.
             // Pour l'instant, on arrête juste le calcul du total.
             break; 
        }
    }
    Ogre::LogManager::getSingleton().logMessage("ScoreManager: Score calculated. Total = " + Ogre::StringConverter::toString(mTotalScore));
}

// Met à jour l'affichage du score à l'écran
void ScoreManager::updateScoreDisplay() {
    if (mScoreText) {
        mScoreText->setCaption("Score: " + Ogre::StringConverter::toString(mTotalScore));
        // Ici, on pourrait ajouter l'affichage détaillé par frame si nécessaire
    }
}

// Retourne le score total actuel
int ScoreManager::getCurrentScore() const {
    return mTotalScore;
}

// --- Fonctions utilitaires privées --- (à déclarer dans le .h)

bool ScoreManager::isStrike(int rollIndex) const {
    return mRolls[rollIndex] == 10;
}

bool ScoreManager::isSpare(int rollIndex) const {
    // Assure que rollIndex+1 est valide avant d'y accéder
    if (rollIndex + 1 < MAX_ROLLS) {
        return mRolls[rollIndex] + mRolls[rollIndex + 1] == 10;
    }
    return false;
}

int ScoreManager::strikeBonus(int rollIndex) const {
    // Assure que les indices sont valides
    if (rollIndex + 2 < MAX_ROLLS) {
        return mRolls[rollIndex + 1] + mRolls[rollIndex + 2];
    }
    return 0; // Pas assez de lancers pour le bonus
}

int ScoreManager::spareBonus(int rollIndex) const {
    // Assure que l'indice est valide
    if (rollIndex + 2 < MAX_ROLLS) {
        return mRolls[rollIndex + 2];
    }
    return 0; // Pas assez de lancers pour le bonus
}

int ScoreManager::openFrameScore(int rollIndex) const {
    // Assure que les indices sont valides
    if (rollIndex + 1 < MAX_ROLLS) {
        return mRolls[rollIndex] + mRolls[rollIndex + 1];
    }
    return 0; // Pas assez de lancers pour le score
}

// --- Fin des fonctions utilitaires ---

