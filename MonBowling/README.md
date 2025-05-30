Si on utilise le lane2.mesh:
                            mLaneNode->yaw(Ogre::Radian(Ogre::Degree(90))); (BowlingLane.cpp)
                            mCameraNode->setPosition(Ogre::Vector3(15, 5, -85)); (Application.cpp)
                            mCameraNode->lookAt(Ogre::Vector3(-3, 10, 80), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z); (Application.cpp)
                            mLane->create(Ogre::Vector3(17.0f, 0.0f, 0.0f));
                            float scale= 4.0f; (BowlingBall.h)
                            Ogre::Vector3 ballPosition(14.0f, 2.0f, -69.0f); (Application.cpp)
                            float pinZOffset = 100.0f; (BowlingLane.cpp)
                            float pinXOffset = -3.0f;
                            Ogre::Vector3 pinBasePosition = Ogre::Vector3(
        mBallStartPosition.x + pinXOffset,    
        1.5f,                   
        mBallStartPosition.z + pinZOffset  
    );
                            float spacing = 1.0f;
                            float scale = 10.0f; (BowlingPin.cpp)
                            arrowNode->setPosition(14.0f, 1.5f, -69.0f); (AimingSystem.cpp)
                             arrowNode->setScale(0.5f, 2.0f, 0.5f);

Si avec polygon8.mesh:
                        mLaneNode->yaw(Ogre::Degree(180));  (BowlingLane.cpp)
                        Ogre::Vector3 ballPosition(0.0f, 0.5f, -7.5f); (Application.cpp)
                        float scale= 1.0f; (BowlingBall.h)
                         mCameraNode->setPosition(Ogre::Vector3(0.0f, 1.0f, -11.0f)); (Application.cpp)
                        mCameraNode->lookAt(Ogre::Vector3(-3, 1, 80), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z); (Application.cpp)
                        mLane->create(Ogre::Vector3(0.0f, 0.5f, -7.5f));
                        float pinZOffset = 17.5f; (BowlingLane.cpp)
                        float pinXOffset = -0.0f;
                        Ogre::Vector3 pinBasePosition = Ogre::Vector3(
        mBallStartPosition.x + pinXOffset,    
        0.0f,                   
        mBallStartPosition.z + pinZOffset  
    );
                        float spacing = 0.01f;
                        float scale = 1.0f; (BowlingPin.cpp)
                        arrowNode->setPosition(0.0f, 0.5f, -7.5f); (AimingSystem.cpp)

