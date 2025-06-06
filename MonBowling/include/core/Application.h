#ifndef APPLICATION_H
#define APPLICATION_H

#include <OgreApplicationContext.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRTShaderSystem.h>
#include <OgreInput.h>
#include <OgreFrameListener.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlaySystem.h>
#include <iostream>
#include <OgreTimer.h>
#include <memory> // Pour std::unique_ptr

#include "../../include/objects/BowlingBall.h"
#include "../../include/objects/BowlingLane.h"
#include "../../include/managers/AudioManager.h"

// Inclusion FMOD (supposant chemin global configuré)
#include <fmod.hpp>
#include <fmod_errors.h>

// Forward declarations pour éviter dépendances cycliques si possible
class BowlingBall;
class BowlingLane;
class GameManager;
class AudioManager;

// Utilisation de l'espace de noms OgreBites pour simplifier
namespace OgreBites {
    class KeyboardEvent;
    class MouseButtonEvent;
    class MouseMotionEvent;
}

class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener {
private:
    // Gestionnaire de scène
    Ogre::SceneManager* scene;

    // Caméra
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mCameraNode;
    Ogre::OverlaySystem* overlaySystem; // Semble non utilisé directement ici

    // Piste de bowling
    std::unique_ptr<BowlingLane> mLane;

    // Boule de bowling
    std::unique_ptr<BowlingBall> mBall;

    // Retrait des états de touches (gérés par GameManager/AimingSystem)
    // bool mKeyW, mKeyA, mKeyS, mKeyD, mKeySpace, mKeyC;

    // Ogre::Timer mCascadeTimer; // Semble non utilisé

public:
    Application();
    virtual ~Application() override; // Utiliser override pour les destructeurs virtuels

    // Configuration de l'application
    virtual void setup() override;

    // Création de la scène
    void createScene();

    // Configuration de la physique
    void setupPhysics();

    // --- Méthodes héritées de OgreBites::ApplicationContext ---
    // Note: shutdown est aussi dans ApplicationContext
    virtual void shutdown() override; // Ajout de la déclaration manquante

    // --- Méthodes héritées de OgreBites::InputListener --- 
    // Gestion des événements de rendu (FrameListener via ApplicationContext)
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

    // Gestion des événements clavier
    virtual bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    virtual bool keyReleased(const OgreBites::KeyboardEvent& evt) override;

    // Gestion des événements souris
    virtual bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
    virtual bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
    virtual bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;

    // --- Méthodes héritées de OgreBites::WindowListener (via ApplicationContext) --- 
    virtual void windowClosed(Ogre::RenderWindow* rw) override; // Ajout de la déclaration manquante

};

#endif 

