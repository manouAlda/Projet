#include "../../include/managers/CameraController.h"
#include <OgreStringConverter.h>
#include <OgreSceneQuery.h>

CameraController::CameraController(Ogre::Camera* camera, Ogre::SceneManager* sceneMgr)
    : mCamera(camera), mSceneMgr(sceneMgr), mCameraMoving(false) {
    mCameraNode = camera->getParentSceneNode();
}

void CameraController::handleMouseClick(const OgreBites::MouseButtonEvent& evt) {
    if (evt.button == OgreBites::BUTTON_LEFT) {
        // Créer un rayon à partir du clic
        Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
            evt.x / (float)mCamera->getViewport()->getActualWidth(),
            evt.y / (float)mCamera->getViewport()->getActualHeight());

        // Log pour déboguer les coordonnées du clic
        Ogre::LogManager::getSingleton().logMessage("Clic écran à : x=" + Ogre::StringConverter::toString(evt.x) + 
                                                    ", y=" + Ogre::StringConverter::toString(evt.y));

        // Configurer le RaySceneQuery
        Ogre::RaySceneQuery* raySceneQuery = mSceneMgr->createRayQuery(mouseRay);
        raySceneQuery->setSortByDistance(true);

        // Exécuter le raycast
        Ogre::RaySceneQueryResult& result = raySceneQuery->execute();
        Ogre::Vector3 targetPoint;
        bool hitFound = false;

        for (const auto& entry : result) {
            if (entry.movable) {
                // Calculer le point exact d'intersection avec l'objet
                targetPoint = mouseRay.getPoint(entry.distance);
                Ogre::LogManager::getSingleton().logMessage("Objet touché : " + entry.movable->getName() + 
                                                            " à distance : " + Ogre::StringConverter::toString(entry.distance) +
                                                            ", point : " + Ogre::StringConverter::toString(targetPoint));
                hitFound = true;
                break;
            }
        }

        if (!hitFound) {
            // Si aucun objet n'est touché, calculer l'intersection avec le plan du sol
            // Ajuste la hauteur du sol selon ta piste (par exemple, y = 0 ou y = 10)
            Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0); // Change 0 si le sol n'est pas à y=0
            std::pair<bool, Ogre::Real> intersect = mouseRay.intersects(groundPlane);
            if (intersect.first) {
                // Point d'intersection avec le sol
                targetPoint = mouseRay.getPoint(intersect.second);
                Ogre::LogManager::getSingleton().logMessage("Sol touché à : " + Ogre::StringConverter::toString(targetPoint));
                hitFound = true;
            }
        }

        if (hitFound) {
            // Calculer la nouvelle position de la caméra (même hauteur)
            Ogre::Vector3 currentPos = mCameraNode->getPosition();
            float distance = (currentPos - targetPoint).length();
            Ogre::Vector3 direction = (targetPoint - currentPos).normalisedCopy();
            Ogre::Vector3 newPos = targetPoint - direction * distance;
            newPos.y = currentPos.y; // Vue horizontale

            // Définir les cibles pour l'animation
            mCameraTargetPos = newPos;
            mCameraTargetLookAt = targetPoint;
            mCameraMoving = true;

            Ogre::LogManager::getSingleton().logMessage("Point ciblé à : " + Ogre::StringConverter::toString(targetPoint));
        } else {
            Ogre::LogManager::getSingleton().logMessage("Aucun point touché.");
        }

        // Nettoyer
        mSceneMgr->destroyQuery(raySceneQuery);
    }
}

void CameraController::update(const Ogre::FrameEvent& evt) {
    if (!mCameraMoving) return;

    // Interpolation fluide de la position
    const float tightness = 15.0f * evt.timeSinceLastFrame; // Vitesse encore plus rapide
    Ogre::Vector3 currentPos = mCameraNode->getPosition();
    Ogre::Vector3 newPos = currentPos + (mCameraTargetPos - currentPos) * tightness;
    mCameraNode->setPosition(newPos);

    // Interpolation de l'orientation
    mCameraNode->lookAt(mCameraTargetLookAt, Ogre::Node::TS_WORLD);

    // Supprimer le pitch pour une vue horizontale
    Ogre::Vector3 xAxis = mCameraNode->getOrientation().xAxis();
    Ogre::Vector3 zAxis = mCameraNode->getOrientation().zAxis();
    mCameraNode->setOrientation(Ogre::Quaternion(xAxis, Ogre::Vector3::UNIT_Y, zAxis));

    // Log pour déboguer la position finale de la caméra
    Ogre::LogManager::getSingleton().logMessage("Caméra à : " + Ogre::StringConverter::toString(newPos) + 
                                                ", regarde : " + Ogre::StringConverter::toString(mCameraTargetLookAt));

    // Arrêter le mouvement si proche de la cible
    if ((newPos - mCameraTargetPos).length() < 0.1f) {
        mCameraMoving = false;
    }
}