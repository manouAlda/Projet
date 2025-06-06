#ifndef BOWLING_LANE_H
#define BOWLING_LANE_H

#include <Ogre.h>
#include <vector>
#include <memory>
#include "../../include/managers/PhysicsManager.h"
#include "../../include/objects/BowlingPin.h"

class BowlingLane {
    private:    
        Ogre::SceneManager* sceneMgr;
        Ogre::Vector3 ballStartPosition;
        
        // Éléments de la piste
        Ogre::SceneNode* laneNode;
        Ogre::Entity* laneEntity;
        
        // Les quilles
        std::vector<std::unique_ptr<BowlingPin>> pins;
        bool pinsInitialized;

        void createLane();
        void setupPins();
        
    public:
        const std::vector<std::unique_ptr<BowlingPin>>& getPins() const;

        BowlingLane(Ogre::SceneManager* sceneMgr);
        ~BowlingLane();
        
        void create(const Ogre::Vector3& ballStartPosition);
        void update(float deltaTime);
        void resetPins();
        int countKnockedDownPins() const;
};

#endif 