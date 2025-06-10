#include "../../include/states/ScoreManager.h"
#include <OgreStringConverter.h>
#include <vector>
#include <numeric>
#include <OgreLogManager.h>

ScoreManager* ScoreManager::mInstance = nullptr;

ScoreManager* ScoreManager::getInstance() {
    if (mInstance == nullptr) {
        mInstance = new ScoreManager();
    }
    return mInstance;
}

ScoreManager::ScoreManager()
    : mScoreOverlay(nullptr),
      mScoreContainer(nullptr),
      mScoreText(nullptr) {
    resetScore(); 
}

ScoreManager::~ScoreManager() {
    if (mScoreOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(mScoreOverlay);
    }
}

void ScoreManager::initialize() {
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    mScoreOverlay = overlayManager.create("ScoreOverlay");
    
    // Création du conteneur pour l'affichage du score
    mScoreContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "ScoreContainer"));
    mScoreContainer->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreContainer->setPosition(20, 20);  
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
    
    resetScore();
}

void ScoreManager::recordRoll(int pins) {
    if (mCurrentRoll < MAX_ROLLS) {
        mRolls[mCurrentRoll++] = pins;
        Ogre::LogManager::getSingleton().logMessage("ScoreManager: Roll " + Ogre::StringConverter::toString(mCurrentRoll) + " recorded: " + Ogre::StringConverter::toString(pins) + " pins.");
        calculateScore(); 
        updateScoreDisplay();
    } else {
        Ogre::LogManager::getSingleton().logMessage("ScoreManager: Maximum rolls reached.");
    }
}

void ScoreManager::resetScore() {
    mRolls.fill(0);
    mFrameScores.fill(0);
    mCurrentRoll = 0;
    mTotalScore = 0;
    if (mScoreText) { 
        updateScoreDisplay();
    }
    Ogre::LogManager::getSingleton().logMessage("ScoreManager: Score reset.");
}

void ScoreManager::calculateScore() {
    mTotalScore = 0;
    mFrameScores.fill(0);
    int rollIndex = 0;
    
    for (int frame = 0; frame < 10; ++frame) {
        if (rollIndex >= mCurrentRoll) break; 

        if (isStrike(rollIndex)) { // Strike
            if (rollIndex + 2 < mCurrentRoll) { 
                 mFrameScores[frame] = 10 + strikeBonus(rollIndex);
                 mTotalScore += mFrameScores[frame];
            }
            rollIndex++;
        } else if (isSpare(rollIndex)) { // Spare
            if (rollIndex + 2 < mCurrentRoll) { 
                mFrameScores[frame] = 10 + spareBonus(rollIndex);
                mTotalScore += mFrameScores[frame];
            }
            rollIndex += 2;
        } else { // Open frame
            if (rollIndex + 1 < mCurrentRoll) { 
                mFrameScores[frame] = openFrameScore(rollIndex);
                mTotalScore += mFrameScores[frame];
            }
            rollIndex += 2;
        }
        
        // Gérer le cas où le calcul s'arrête car les lancers suivants n'ont pas été faits
        if (rollIndex >= mCurrentRoll && frame < 9) {
             break; 
        }
    }
    Ogre::LogManager::getSingleton().logMessage("ScoreManager: Score calculated. Total = " + Ogre::StringConverter::toString(mTotalScore));
}

void ScoreManager::updateScoreDisplay() {
    if (mScoreText) {
        mScoreText->setCaption("Score: " + Ogre::StringConverter::toString(mTotalScore));
    }
}

int ScoreManager::getCurrentScore() const {
    return mTotalScore;
}

bool ScoreManager::isStrike(int rollIndex) const {
    return mRolls[rollIndex] == 10;
}

bool ScoreManager::isSpare(int rollIndex) const {
    if (rollIndex + 1 < MAX_ROLLS) {
        return mRolls[rollIndex] + mRolls[rollIndex + 1] == 10;
    }
    return false;
}

int ScoreManager::strikeBonus(int rollIndex) const {
    if (rollIndex + 2 < MAX_ROLLS) {
        return mRolls[rollIndex + 1] + mRolls[rollIndex + 2];
    }
    return 0;
}

int ScoreManager::spareBonus(int rollIndex) const {
    if (rollIndex + 2 < MAX_ROLLS) {
        return mRolls[rollIndex + 2];
    }
    return 0; // Pas assez de lancers pour le bonus
}

int ScoreManager::openFrameScore(int rollIndex) const {
    if (rollIndex + 1 < MAX_ROLLS) {
        return mRolls[rollIndex] + mRolls[rollIndex + 1];
    }
    return 0; // Pas assez de lancers pour le score
}
