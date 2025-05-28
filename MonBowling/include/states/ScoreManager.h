#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <vector>
#include <memory>
#include <array> // Include <array>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreTechnique.h>
#include <OgrePass.h>

// Pattern Singleton pour le gestionnaire de score
class ScoreManager {
public:
    // Obtenir l'instance unique
    static ScoreManager* getInstance();
    
    // Initialisation du gestionnaire de score et de l'UI
    void initialize();
    
    // Enregistre le nombre de quilles abattues pour le lancer actuel
    void recordRoll(int pins);
    
    // Réinitialisation du score pour une nouvelle partie
    void resetScore();
    
    // Obtenir le score total actuel
    int getCurrentScore() const;
    
    // Mise à jour de l'affichage du score à l'écran
    void updateScoreDisplay();

private:
    // Constructeur privé (pattern Singleton)
    ScoreManager();
    
    // Destructeur
    ~ScoreManager();
    
    // Instance unique
    static ScoreManager* mInstance;

    // Constante pour le nombre maximum de lancers possibles dans une partie (10 frames * 2 lancers + 3 pour la 10e frame max)
    static const int MAX_ROLLS = 21; 

    // Tableau pour stocker les quilles abattues à chaque lancer
    std::array<int, MAX_ROLLS> mRolls;
    
    // Tableau pour stocker le score calculé pour chaque frame (optionnel, pour affichage détaillé)
    std::array<int, 10> mFrameScores;
    
    // Index du prochain lancer à enregistrer dans mRolls
    int mCurrentRoll;
    
    // Score total calculé
    int mTotalScore;
    
    // Éléments d'interface pour l'affichage du score
    Ogre::Overlay* mScoreOverlay;
    Ogre::OverlayContainer* mScoreContainer;
    Ogre::TextAreaOverlayElement* mScoreText;

    // --- Méthodes utilitaires privées pour le calcul du score ---
    void calculateScore();
    bool isStrike(int rollIndex) const;
    bool isSpare(int rollIndex) const;
    int strikeBonus(int rollIndex) const;
    int spareBonus(int rollIndex) const;
    int openFrameScore(int rollIndex) const;
};

#endif // SCOREMANAGER_H

