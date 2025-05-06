// main.cpp - Point d'entrée du jeu d'infiltration
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreWindowEventUtilities.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>

#include <OgreRTShaderSystem.h>

#include <OIS/OIS.h>

#include "GameManager.h"

// Classe principale du jeu
class InfiltrationGame : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener 
{
private:
    Ogre::Root* mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
    OIS::InputManager* mInputManager;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse* mMouse;

    // Notre gestionnaire de jeu
    GameManager* mGameManager;

    // Le jeu est-il en cours d'exécution?
    bool mRunning;

public:
    InfiltrationGame() :
        mRoot(0),
        mResourcesCfg(Ogre::StringUtil::BLANK),
        mPluginsCfg(Ogre::StringUtil::BLANK),
        mWindow(0),
        mSceneMgr(0),
        mCamera(0),
        mInputManager(0),
        mKeyboard(0),
        mMouse(0),
        mGameManager(0),
        mRunning(true)
    {
    }

    ~InfiltrationGame()
    {
        // Nettoyage
        Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
        windowClosed(mWindow);

        delete mGameManager;

        delete mRoot;
    }

    bool setup()
    {
        // Chemins de configuration
        #ifdef _DEBUG
        mResourcesCfg = "resources_d.cfg";
        mPluginsCfg = "plugins_d.cfg";
        #else
        mResourcesCfg = "resources.cfg";
        mPluginsCfg = "plugins.cfg";
        #endif

        // Création de la root Ogre
        mRoot = new Ogre::Root(mPluginsCfg);

        // Configuration des ressources
        configureResources();

        // Configuration de la fenêtre
        if (!configureRTShader()) {
            return false;
        }

        // Configurer la fenêtre
        if (!configureOgre()) {
            return false;
        }

        // Initialiser OIS
        setupInput();

        // Créer le gestionnaire de scène
        mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

        // Créer la caméra
        createCamera();

        // Créer le GameManager
        mGameManager = new GameManager(mSceneMgr, mCamera, mWindow);
        mGameManager->setupGame();

        // Ajouter l'écouteur de frame
        mRoot->addFrameListener(this);

        // Ajouter l'écouteur de fenêtre
        Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

        return true;
    }

    void configureResources()
    {
        // Charger les ressources
        Ogre::ConfigFile cf;
        cf.load(mResourcesCfg);

        // Parcourir les sections
        Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
        Ogre::String secName, typeName, archName;

        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
            Ogre::ConfigFile::SettingsMultiMap::iterator i;

            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
            }
        }

        // Initialiser les ressources
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    }

    bool configureRTShader()
    {
        if (Ogre::RTShader::ShaderGenerator::initialize())
        {
            return true;
        }
        return false;
    }

    bool configureOgre()
    {
        // Afficher la boîte de dialogue de configuration si nécessaire
        if (!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
        {
            return false;
        }

        // Créer la fenêtre
        mWindow = mRoot->initialise(true, "Jeu d'Infiltration");

        return true;
    }

    void setupInput()
    {
        OIS::ParamList pl;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        mWindow->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

        mInputManager = OIS::InputManager::createInputSystem(pl);

        mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
        mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

        mKeyboard->setEventCallback(this);
        mMouse->setEventCallback(this);

        // Définir les dimensions de la souris
        unsigned int width, height, depth;
        int left, top;
        mWindow->getMetrics(width, height, depth, left, top);
        const OIS::MouseState& ms = mMouse->getMouseState();
        ms.width = width;
        ms.height = height;
    }

    void createCamera()
    {
        // Créer la caméra
        mCamera = mSceneMgr->createCamera("PlayerCamera");
        mCamera->setPosition(Ogre::Vector3(0, 10, 50));
        mCamera->lookAt(Ogre::Vector3(0, 0, 0));
        mCamera->setNearClipDistance(5);

        // Créer un viewport
        Ogre::Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
        mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
    }

    void run()
    {
        // Boucle principale
        while (mRunning)
        {
            // Pomper les messages Windows
            Ogre::WindowEventUtilities::messagePump();

            // Rendre une frame
            if (!mRoot->renderOneFrame()) {
                mRunning = false;
            }
        }
    }

    // Gestionnaires d'événements Ogre
    bool frameRenderingQueued(const Ogre::FrameEvent& evt)
    {
        if (mWindow->isClosed()) {
            return false;
        }

        // Mettre à jour OIS
        mKeyboard->capture();
        mMouse->capture();

        // Mettre à jour la logique du jeu
        mGameManager->update(evt.timeSinceLastFrame);

        return true;
    }

    void windowClosed(Ogre::RenderWindow* rw)
    {
        if (rw == mWindow)
        {
            if (mInputManager)
            {
                mInputManager->destroyInputObject(mKeyboard);
                mInputManager->destroyInputObject(mMouse);
                OIS::InputManager::destroyInputSystem(mInputManager);
                mInputManager = 0;
            }
        }
    }

    // Gestionnaires d'événements OIS
    bool keyPressed(const OIS::KeyEvent& evt)
    {
        if (evt.key == OIS::KC_ESCAPE) {
            mRunning = false;
        }

        mGameManager->keyPressed(evt);
        return true;
    }

    bool keyReleased(const OIS::KeyEvent& evt)
    {
        mGameManager->keyReleased(evt);
        return true;
    }

    bool mouseMoved(const OIS::MouseEvent& evt)
    {
        mGameManager->mouseMoved(evt);
        return true;
    }

    bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
    {
        mGameManager->mousePressed(evt, id);
        return true;
    }

    bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
    {
        mGameManager->mouseReleased(evt, id);
        return true;
    }
};

// Point d'entrée principal
int main(int argc, char *argv[])
{
    try
    {
        InfiltrationGame app;
        if (app.setup()) {
            app.run();
        }
    }
    catch (Ogre::Exception& e)
    {
        std::cerr << "Une exception Ogre s'est produite: " << e.getFullDescription().c_str() << std::endl;
    }

    return 0;
}