#include <Ogre.h>
#include <OgreApplicationContext.h>
#include "Physics.hpp"
#include "MouseHandler.hpp"
#include "Player.hpp"

class World : public OgreBites::ApplicationContext, public OgreBites::InputListener {
    private:
        Physics* physics;
        Ogre::SceneManager* sceneMgr;
        MouseHandler* mouseHandler;
        Player* player;

        // Propriétés pour la caméra qui suit le joueur
        Ogre::Camera* mCamera;
        Ogre::SceneNode* mCameraNode;
        bool mCameraFollowPlayer;
        Ogre::Vector3 mCameraOffset;

    public:
        World();
        ~World();

        void setup() override;
        void configurationFenetre();
        bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

        void setupCamera();
        void updateCamera(float deltaTime);

        // Vous pourriez également ajouter des méthodes pour gérer les entrées du joueur
        bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
        bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
};