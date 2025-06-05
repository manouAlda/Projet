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

#include <fmod.hpp>
#include <fmod_errors.h>

#include "../objects/BowlingBall.h"
#include "../objects/BowlingLane.h"
#include "GameManager.h"

class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener {
    private:
        // Gestionnaire de scène
        Ogre::SceneManager* scene;
        
        // Caméra
        Ogre::Camera* mCamera;
        Ogre::SceneNode* mCameraNode;
        Ogre::OverlaySystem* overlaySystem;
        
        // Piste de bowling
        std::unique_ptr<BowlingLane> mLane;
        
        // Boule de bowling
        std::unique_ptr<BowlingBall> mBall;
        
        // État des touches
        bool mKeyW, mKeyA, mKeyS, mKeyD, mKeySpace, mKeyC;

        Ogre::Timer mCascadeTimer;

    public:
        Application();
        virtual ~Application();
        
        // Configuration de l'application
        virtual void setup() override;
        
        // Création de la scène
        void createScene();
        
        // Configuration de la physique
        void setupPhysics();
        
        // Gestion des événements de rendu
        virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
        
        // Gestion des événements clavier
        virtual bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
        virtual bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
        
        // Gestion des événements souris
        virtual bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
        virtual bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
        virtual bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;

        // void checkFMOD(FMOD_RESULT result);
        // void initAudio();
        // void playSound();
        // void updateAudio();
        // void cleanAudio();
        // void initFMOD();
        // void updateFMOD();
        // void shutdownFMOD();
        

};

#endif // APPLICATION_H
