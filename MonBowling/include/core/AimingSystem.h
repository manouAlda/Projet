#ifndef AIMINGSYSTEM_H
#define AIMINGSYSTEM_H

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <Ogre.h>
#include <OgreQuaternion.h>
#include <OgreRay.h>
#include <OgreCamera.h>
#include <OgreInput.h>

class AimingSystem {
    private: 
        // Références aux objets externes
        Ogre::SceneManager* scene;
        Ogre::Camera* camera;
        
        // Éléments de la flèche de visée
        Ogre::SceneNode* arrowNode;
        Ogre::Entity* arrowEntity;
        
        // Éléments de l'interface utilisateur (overlay)
        Ogre::Overlay* gameOverlay;
        Ogre::OverlayContainer* powerBarContainer;
        Ogre::OverlayElement* mPowerBarBackground;
        Ogre::OverlayElement* mPowerBarFill;
        Ogre::OverlayElement* mSpinEffectControl;
        Ogre::OverlayElement* mSpinEffectIndicator;
        Ogre::OverlayContainer* mScoreContainer;
        Ogre::TextAreaOverlayElement* mScoreDisplay;
        
        // État du système de visée
        bool mAimingActive;
        Ogre::Vector3 mAimingDirection;
        
        // État de la barre de puissance
        bool mPowerBarActive;
        float mPowerValue;
        float mPowerDirection;  // 1 pour croissant, -1 pour décroissant
        float mPowerSpeed;      // Vitesse de remplissage
        
        // État de l'effet
        float mSpinEffect;      // -1.0 à 1.0 (gauche à droite)
        
        // Constantes
        const float MAX_POWER = 100.0f;
        const float MIN_POWER = 5.0f;

         // Création des éléments visuels
         void createAimingArrow();
         void createPowerBar();
         void createSpinEffectControl();
         
         // Mise à jour des éléments visuels
         void updateAimingArrow();
         void updatePowerBar(float deltaTime);
         void updateSpinEffectControl();

    public:
        AimingSystem(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera);
        ~AimingSystem();

        // Initialisation du système de visée
        void initialize();
        
        // Mise à jour du système de visée
        void update(float deltaTime);
        
        // Gestion des événements souris
        void handleMouseMove(const OgreBites::MouseMotionEvent& evt);
        void handleMouseClick(const OgreBites::MouseButtonEvent& evt);
        
        // Gestion de la visée
        void setAimingDirection(const Ogre::Vector3& direction);
        Ogre::Vector3 getAimingDirection() const;
        
        // Gestion de la puissance
        void startPowerBar();
        void stopPowerBar();
        float getPower() const;
        bool isPowerBarActive() const;
        
        // Gestion de l'effet
        void setSpinEffect(float effect);
        float getSpinEffect() const;
        
        // Gestion de l'annulation
        void resetAiming();
        
        // État du système
        bool isAimingActive() const;
        void setAimingActive(bool active);
        
        // Accesseurs pour les éléments d'interface
        Ogre::SceneNode* getArrowNode() const;
        Ogre::OverlayElement* getPowerBarElement() const;
        Ogre::OverlayElement* getSpinEffectElement() const;
        Ogre::OverlayElement* getCancelButtonElement() const;

};

#endif // AIMINGSYSTEM_H
