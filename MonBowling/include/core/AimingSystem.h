#ifndef AIMING_SYSTEM_H
#define AIMING_SYSTEM_H

#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <Ogre.h>
#include <OgreEntity.h>
#include <OgreQuaternion.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgrePanelOverlayElement.h>
#include <OgreInput.h>
#include <OgreLogManager.h>

// Constantes pour la puissance et le spin
const float MIN_POWER = 0.0f;
const float MAX_POWER = 100.0f; 
const float POWER_INCREMENT = 25.0f; // Augmentation de puissance par seconde (ou par appui)
const float MIN_SPIN = -1.0f;
const float MAX_SPIN = 1.0f;
const float SPIN_INCREMENT = 1.5f; // Augmentation de spin par seconde (ou par appui)

class AimingSystem {
    private:
        void createAimingArrow();
        void createOverlays(); // Fonction regroupant la création des overlays
        void updateAimingArrow();
        void updatePowerBarDisplay(); 
        void updateSpinIndicatorDisplay(); 

        Ogre::SceneManager* scene;
        Ogre::Camera* camera;

        // Éléments de visée 3D
        Ogre::SceneNode* arrowNode;
        Ogre::Entity* arrowEntity;

        // Overlays
        Ogre::Overlay* gameOverlay;
        // Barre de puissance
        Ogre::OverlayContainer* powerBarContainer;
        Ogre::OverlayElement* mPowerBarBackground;
        Ogre::OverlayElement* mPowerBarFill; // L'élément qui représente le remplissage
        // Contrôle de spin
        Ogre::OverlayContainer* mSpinContainer; // Conteneur global pour le spin
        Ogre::OverlayElement* mSpinEffectControl; // Fond de la barre de spin
        Ogre::OverlayElement* mSpinEffectIndicator; // Indicateur de spin

        // État du système
        bool mAimingActive;       // La visée (direction) est-elle active ?
        bool mPowerInputActive;   // L'entrée pour la puissance/spin est-elle active ?

        // Valeurs de visée/lancement
        Ogre::Vector3 mAimingDirection;
        float mPowerValue;        // Puissance actuelle (0-MAX_POWER)
        float mSpinEffect;        // Effet actuel (-1.0 à 1.0)

        // Variables pour la gestion des touches maintenues (optionnel, si on veut une augmentation continue)
        // bool mKeyUpPressed;
        // bool mKeyDownPressed;
        // bool mKeyLeftPressed;
        // bool mKeyRightPressed;

    public:
        AimingSystem(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera);
        ~AimingSystem();

        void initialize();
        void update(float deltaTime); 

        // Gestion des entrées (appelé par GameManager)
        void handleKeyPress(const OgreBites::KeyboardEvent& evt);
        void handleKeyRelease(const OgreBites::KeyboardEvent& evt);

        // Contrôle de l'état
        void setAimingActive(bool active);
        bool isAimingActive() const;
        void startPowerPhase(); // Appelé quand on passe à l'état POWER
        void resetAiming();     // Réinitialise tout (direction, puissance, spin)

        // Getters pour le lancement
        Ogre::Vector3 getAimingDirection() const;
        float getPower() const;
        float getSpinEffect() const;

        // Méthodes pour ajuster puissance/spin (appelées par handleKeyPress)
        void adjustPower(float delta);
        void adjustSpin(float delta);
};

#endif 

