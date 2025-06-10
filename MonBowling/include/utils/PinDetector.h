#ifndef PINDETECTOR_H
#define PINDETECTOR_H

#include <vector>
#include <memory>
#include <OgreTimer.h>
#include <OgreStringConverter.h>
#include <OgreLogManager.h>
#include "../include/objects/BowlingPin.h"

// Classe pour la détection des quilles tombées
class PinDetector {
    private:
        // Référence aux quilles
        const std::vector<std::unique_ptr<BowlingPin>>* mPins;
        
        // État de la détection
        bool mDetectionActive;
        bool mDetectionComplete;
        
        // Minuteur pour le délai de cascade
        Ogre::Timer mCascadeTimer;
        
        // Délai de cascade (en millisecondes)
        const unsigned long CASCADE_DELAY = 200; // 5 secondes (5000)
        
        // Nombre de quilles tombées
        int mKnockedDownPinCount;
        
        // État précédent des quilles (pour détecter les changements)
        std::vector<bool> mPreviousPinStates;


    public:
        PinDetector();
        ~PinDetector();
        
        // Initialisation du détecteur
        void initialize(const std::vector<std::unique_ptr<BowlingPin>>& pins);
        
        // Démarrer la détection après un lancer
        void startDetection();
        
        // Mettre à jour la détection
        void update(float deltaTime);
        
        // Obtenir le nombre de quilles tombées
        int getKnockedDownPinCount() const;
        
        // Vérifier si la détection est terminée
        bool isDetectionComplete() const;
        
        // Réinitialiser la détection
        void reset();
};

#endif // PINDETECTOR_H
