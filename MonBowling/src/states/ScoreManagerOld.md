#include "../../include/states/ScoreManager.h"
#include <OgreStringConverter.h>

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
    : mCurrentScore(0),
      mThrowCount(0),
      mScoreOverlay(nullptr),
      mScoreContainer(nullptr),
      mScoreText(nullptr) {
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
    mScoreContainer->setPosition(20, 20);  // En haut à droite
    mScoreContainer->setDimensions(200, 30);
    
    // Création du texte pour l'affichage du score
    mScoreText = static_cast<Ogre::TextAreaOverlayElement*>(
        overlayManager.createOverlayElement("TextArea", "ScoreText"));
    mScoreText->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreText->setPosition(0, 0);
    mScoreText->setDimensions(200, 30);
    mScoreText->setCharHeight(24);
    mScoreText->setFontName("Arial");
    mScoreText->setColour(Ogre::ColourValue::White);
    mScoreText->setCaption("Score: 0");
    
    // Ajout du texte au conteneur
    mScoreContainer->addChild(mScoreText);
    
    // Ajout du conteneur à l'overlay
    mScoreOverlay->add2D(mScoreContainer);
    
    // Affichage de l'overlay
    mScoreOverlay->setZOrder(100);
    mScoreOverlay->show();
    
    // Réinitialisation du score
    resetScore();
}

void ScoreManager::updateScore(int pinsKnockedDown) {
    // Mise à jour du score
    mCurrentScore += pinsKnockedDown;
    
    // Incrémentation du nombre de lancers
    mThrowCount++;
    
    // Mise à jour de l'affichage
    updateScoreDisplay();
}

void ScoreManager::resetScore() {
    // Réinitialisation du score et du nombre de lancers
    mCurrentScore = 0;
    mThrowCount = 0;
    
    // Mise à jour de l'affichage
    updateScoreDisplay();
}

int ScoreManager::getCurrentScore() const {
    return mCurrentScore;
}

int ScoreManager::getThrowCount() const {
    return mThrowCount;
}

void ScoreManager::updateScoreDisplay() {
    // Mise à jour du texte d'affichage du score
    if (mScoreText) {
        mScoreText->setCaption("Score: " + Ogre::StringConverter::toString(mCurrentScore));
    }
}
