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

#include "../objects/BowlingBall.h"
#include "../objects/BowlingLane.h"
#include "../objects/ObjectFactory.h"
#include "../managers/CameraController.h"
#include "GameManager.h"

class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener {
    private:
        // Gestionnaire de scène
        Ogre::SceneManager* mSceneMgr;
        
        // Caméra
        Ogre::Camera* mCamera;
        Ogre::SceneNode* mCameraNode;
        Ogre::OverlaySystem* overlaySystem;
        
        // Contrôleur de caméra
        std::unique_ptr<CameraController> mCameraController;
        
        // Factory d'objets
        std::unique_ptr<ObjectFactory> mObjectFactory;
        
        // Piste de bowling
        std::unique_ptr<BowlingLane> mLane;
        
        // Boule de bowling
        std::unique_ptr<BowlingBall> mBall;
        
        // État des touches
        bool mKeyW, mKeyA, mKeyS, mKeyD, mKeySpace, mKeyC;

    public:
        Application();
        virtual ~Application();
        
        // Configuration de l'application
        virtual void setup() override;
        
        // Création de la scène
        void createScene();
        
        // Création d'objets dynamiques
        void createDynamicObjects();
        
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

};

#endif // APPLICATION_H
