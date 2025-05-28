#include "../../include/utils/PinDetector.h"

PinDetector::PinDetector()
    : mPins(nullptr),
      mDetectionActive(false),
      mDetectionComplete(false),
      mKnockedDownPinCount(0) {
}

PinDetector::~PinDetector() {
}

void PinDetector::initialize(const std::vector<std::unique_ptr<BowlingPin>>& pins) {
    // Stockage de la référence aux quilles
    mPins = &pins;
    
    // Initialisation de l'état précédent des quilles
    mPreviousPinStates.resize(pins.size(), false);
    
    // Réinitialisation de la détection
    reset();
}

void PinDetector::startDetection() {
    // Activation de la détection
    mDetectionActive = true;
    mDetectionComplete = false;
    
    // Réinitialisation du compteur de quilles tombées
    mKnockedDownPinCount = 0;
    
    // Réinitialisation de l'état précédent des quilles
    if (mPins) {
        mPreviousPinStates.resize(mPins->size(), false);
        for (size_t i = 0; i < mPins->size(); ++i) {
            mPreviousPinStates[i] = false;
        }
    }
    
    // Démarrage du minuteur pour le délai de cascade
    mCascadeTimer.reset();
    
    Ogre::LogManager::getSingleton().logMessage("Détection des quilles démarrée");
}

void PinDetector::update(float deltaTime) {
    if (!mDetectionActive || mDetectionComplete || !mPins) {
        return;
    }
    
    // Vérification du délai de cascade
    if (mCascadeTimer.getMilliseconds() >= CASCADE_DELAY) {
        // Comptage des quilles tombées
        mKnockedDownPinCount = 0;
        
        for (size_t i = 0; i < mPins->size(); ++i) {
            const auto& pin = (*mPins)[i];
            
            if (pin && pin->isKnockedDown()) {
                mKnockedDownPinCount++;
                
                // Mise à jour de l'état précédent
                mPreviousPinStates[i] = true;
            }
        }
        
        // Fin de la détection
        mDetectionComplete = true;
        mDetectionActive = false;
        
        Ogre::LogManager::getSingleton().logMessage("Détection des quilles terminée. Nombre de quilles tombées : " + 
                                                   Ogre::StringConverter::toString(mKnockedDownPinCount));
    } else {
        // Pendant le délai de cascade, on peut suivre les quilles qui tombent pour le débogage
        int currentKnockedDown = 0;
        
        for (size_t i = 0; i < mPins->size(); ++i) {
            const auto& pin = (*mPins)[i];
            
            if (pin && pin->isKnockedDown()) {
                currentKnockedDown++;
                
                // Détection d'une nouvelle quille tombée
                if (!mPreviousPinStates[i]) {
                    mPreviousPinStates[i] = true;
                    Ogre::LogManager::getSingleton().logMessage("Quille " + Ogre::StringConverter::toString(i+1) + " tombée");
                }
            }
        }
        
        // Log pour le débogage
        if (mCascadeTimer.getMilliseconds() % 500 < 20) {  // Log toutes les 500ms environ
            Ogre::LogManager::getSingleton().logMessage("Détection en cours... Temps écoulé : " + 
                                                       Ogre::StringConverter::toString(mCascadeTimer.getMilliseconds()) + 
                                                       " ms, Quilles tombées : " + 
                                                       Ogre::StringConverter::toString(currentKnockedDown));
        }
    }
}

int PinDetector::getKnockedDownPinCount() const {
    return mKnockedDownPinCount;
}

bool PinDetector::isDetectionComplete() const {
    return mDetectionComplete;
}

void PinDetector::reset() {
    // Réinitialisation de l'état de la détection
    mDetectionActive = false;
    mDetectionComplete = false;
    mKnockedDownPinCount = 0;
    
    // Réinitialisation de l'état précédent des quilles
    if (mPins) {
        for (size_t i = 0; i < mPreviousPinStates.size(); ++i) {
            mPreviousPinStates[i] = false;
        }
    }
}
