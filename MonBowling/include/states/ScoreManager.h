#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <vector>
#include <memory>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>

// Pattern Singleton pour le gestionnaire de score
class ScoreManager {
public:
    // Obtenir l'instance unique
    static ScoreManager* getInstance();
    
    // Initialisation du gestionnaire de score
    void initialize();
    
    // Mise à jour du score
    void updateScore(int pinsKnockedDown);
    
    // Réinitialisation du score
    void resetScore();
    
    // Obtenir le score actuel
    int getCurrentScore() const;
    
    // Obtenir le nombre de lancers effectués
    int getThrowCount() const;
    
    // Mise à jour de l'affichage du score
    void updateScoreDisplay();

private:
    // Constructeur privé (pattern Singleton)
    ScoreManager();
    
    // Destructeur
    ~ScoreManager();
    
    // Instance unique
    static ScoreManager* mInstance;
    
    // Score actuel
    int mCurrentScore;
    
    // Nombre de lancers effectués
    int mThrowCount;
    
    // Éléments d'interface pour l'affichage du score
    Ogre::Overlay* mScoreOverlay;
    Ogre::TextAreaOverlayElement* mScoreText;
};

#endif // SCOREMANAGER_H
