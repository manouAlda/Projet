#include "../../include/core/AimingSystem.h"
#include <OgreMaterialManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreResourceGroupManager.h>
#include <OgreStringConverter.h>
#include <algorithm>

AimingSystem::AimingSystem(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera)
    : scene(sceneMgr),
      camera(camera),
      arrowNode(nullptr),
      arrowEntity(nullptr),
      gameOverlay(nullptr),
      powerBarContainer(nullptr),
      mPowerBarBackground(nullptr),
      mPowerBarFill(nullptr),
      mSpinEffectControl(nullptr),
      mSpinEffectIndicator(nullptr),
      mScoreContainer(nullptr),
      mScoreDisplay(nullptr),
      mAimingActive(false),
      mAimingDirection(Ogre::Vector3(0, 0, -1)),
      mPowerBarActive(false),
      mPowerValue(0.0f),
      mPowerDirection(1),
      mPowerSpeed(50.0f),
      mSpinEffect(0.0f)
{
}

AimingSystem::~AimingSystem(){
    if (gameOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(gameOverlay);
    }
    
    if (arrowNode) {
        arrowNode->detachAllObjects();
        scene->destroySceneNode(arrowNode);
    }
    
    if (arrowEntity) {
        scene->destroyEntity(arrowEntity);
    }
}

void AimingSystem::initialize(){
    createAimingArrow();
    createPowerBar();
    createSpinEffectControl();
    
    // Activation du système de visée
    setAimingActive(true);
}

void AimingSystem::createAimingArrow(){
    arrowEntity = scene->createEntity("AimingArrow", "cone.mesh");

    arrowEntity->setMaterialName("AimingArrowMaterial");
    
    // Création du nœud pour la flèche
    arrowNode = scene->getRootSceneNode()->createChildSceneNode("AimingArrowNode");
    arrowNode->attachObject(arrowEntity);
    
    // Orientation et mise à l'échelle de la flèche
    arrowNode->setScale(0.05f, 0.2f, 0.05f);
    arrowNode->pitch(Ogre::Degree(-90));  // Orienter la flèche vers l'avant
    
    arrowNode->setPosition(0.0f, 0.05f, -7.5f); 
}

void AimingSystem::createPowerBar(){
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    if (!gameOverlay) {
        gameOverlay = overlayManager.create("GameOverlay");
    }
    
    // Création du conteneur pour la barre de puissance
    powerBarContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "PowerBarContainer"));
    powerBarContainer->setMetricsMode(Ogre::GMM_PIXELS);

    powerBarContainer->setPosition(20, 100);
    powerBarContainer->setDimensions(30, 200);
      
    // Création de l'arrière-plan de la barre de puissance
    mPowerBarBackground = overlayManager.createOverlayElement("Panel", "PowerBarBackground");
    mPowerBarBackground->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarBackground->setPosition(0, 0);
    mPowerBarBackground->setDimensions(30, 200);

    mPowerBarBackground->setMaterialName("Aiming/PowerBarBackgroundMaterial");

    // Assemblage de la barre de puissance
    powerBarContainer->addChild(mPowerBarBackground);
    
    // Ajout à l'overlay principal
    gameOverlay->add2D(powerBarContainer);
    
    // Masquer la barre de puissance initialement
    //powerBarContainer->hide();
    
    // Affichage de l'overlay
    gameOverlay->setZOrder(100);
    gameOverlay->show();
}

void AimingSystem::createSpinEffectControl(){
    // Création du conteneur pour le contrôle d'effet
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    // Création du conteneur pour le curseur d'effet
    Ogre::OverlayContainer* spinContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "SpinEffectContainer"));
    spinContainer->setMetricsMode(Ogre::GMM_PIXELS);
    spinContainer->setPosition(70, 100);
    spinContainer->setDimensions(200, 30);
    
    // Création de l'arrière-plan du curseur d'effet
    mSpinEffectControl = overlayManager.createOverlayElement("Panel", "SpinEffectControl");
    mSpinEffectControl->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinEffectControl->setPosition(0, 0);
    mSpinEffectControl->setDimensions(200, 30);

    mSpinEffectControl->setMaterialName("SpinEffectBackgroundMaterial");
    
    // Création de l'indicateur du curseur d'effet
    mSpinEffectIndicator = overlayManager.createOverlayElement("Panel", "SpinEffectIndicator");
    mSpinEffectIndicator->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinEffectIndicator->setPosition(95, 0);  // Position centrale
    mSpinEffectIndicator->setDimensions(10, 30);

    mSpinEffectIndicator->setMaterialName("SpinEffectIndicatorMaterial");
    
    // Assemblage du curseur d'effet
    spinContainer->addChild(mSpinEffectControl);
    spinContainer->addChild(mSpinEffectIndicator);
    
    // Ajout à l'overlay principal
    gameOverlay->add2D(spinContainer);
}

void AimingSystem::update(float deltaTime){
    if (mAimingActive) {
        updateAimingArrow();
        
        if (mPowerBarActive) {
            updatePowerBar(deltaTime);
        }
        
        updateSpinEffectControl();
    }
}

void AimingSystem::updateAimingArrow(){
    if (arrowNode) {
        Ogre::Vector3 direction = mAimingDirection.normalisedCopy();
        
        // Calcul de la rotation pour orienter la flèche
        Ogre::Quaternion rotation = Ogre::Vector3::UNIT_Z.getRotationTo(direction);
        arrowNode->setOrientation(rotation);
    }
}

void AimingSystem::updatePowerBar(float deltaTime){
    if (mPowerBarActive && mPowerBarFill) {
        mPowerValue += mPowerDirection * mPowerSpeed * deltaTime;
        
        if (mPowerValue >= MAX_POWER) { 
            mPowerValue = MAX_POWER;
            mPowerDirection = -1;
        } else if (mPowerValue <= MIN_POWER) { 
            mPowerValue = MIN_POWER;
            mPowerDirection = 1;
        }
        
        float fillHeight = (mPowerValue / MAX_POWER) * 200.0f; 
        mPowerBarFill->setPosition(0, 200.0f - fillHeight);
        mPowerBarFill->setDimensions(30, fillHeight);
     
        // Changement de couleur en fonction de la puissance
        Ogre::MaterialPtr fillMaterial = Ogre::MaterialManager::getSingleton().getByName("PowerBarFillMaterial");
        if (fillMaterial) {
            Ogre::Pass* pass = fillMaterial->getTechnique(0)->getPass(0);
            float ratio = mPowerValue / MAX_POWER; 
            pass->setDiffuse(Ogre::ColourValue(ratio, 1.0f - ratio, 0.0f, 0.8f));
        }
    }
}

void AimingSystem::updateSpinEffectControl(){
    if (mSpinEffectIndicator) {
        // Conversion de la valeur d'effet (-1.0 à 1.0) en position (0 à 200)
        float position = ((mSpinEffect + 1.0f) / 2.0f) * 190.0f;
        mSpinEffectIndicator->setPosition(position, 0);
    }
}

void AimingSystem::handleMouseMove(const OgreBites::MouseMotionEvent& evt){
    if (mAimingActive) {
        // Gestion du mouvement de la souris pour la visée
        // Conversion des coordonnées de la souris en rayon dans l'espace 3D
        Ogre::Ray mouseRay = camera->getCameraToViewportRay(
            evt.x / (float)camera->getViewport()->getActualWidth(),
            evt.y / (float)camera->getViewport()->getActualHeight());
        
        // Intersection avec le plan du sol (y = 0)
        Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0);
        std::pair<bool, Ogre::Real> intersection = mouseRay.intersects(groundPlane);
        
        if (intersection.first) {
            // Point d'intersection avec le sol
            Ogre::Vector3 hitPoint = mouseRay.getPoint(intersection.second);
            
            // Calcul de la direction de visée (depuis la position de la boule vers le point d'intersection)
            Ogre::Vector3 ballPosition(0.0f, 0.25f, -7.5f);  // À ajuster selon la position réelle de la boule
            Ogre::Vector3 direction = (hitPoint - ballPosition).normalisedCopy();
            
            // Mise à jour de la direction de visée
            setAimingDirection(direction);
        }
        
        // Gestion du curseur d'effet
        // Vérification si la souris est sur le curseur d'effet
        if (evt.x >= 70 && evt.x <= 270 && evt.y >= 100 && evt.y <= 130) {
            // Conversion de la position de la souris en valeur d'effet (-1.0 à 1.0)
            float effect = ((evt.x - 70) / 200.0f) * 2.0f - 1.0f;
            setSpinEffect(effect);
        }
    }
}

void AimingSystem::handleMouseClick(const OgreBites::MouseButtonEvent& evt){
    if (mAimingActive) {
        // Gestion du clic de souris
        if (evt.button == OgreBites::BUTTON_LEFT) {
            // Vérification si le clic est sur le bouton d'annulation
            if (evt.x >= 20 && evt.x <= 120 && evt.y >= 320 && evt.y <= 360) {
                // Annulation du lancer
                resetAiming();
            }
            // Vérification si le clic est sur la barre de puissance
            else if (evt.x >= 20 && evt.x <= 50 && evt.y >= 100 && evt.y <= 300) {
                // Arrêt de la barre de puissance
                if (mPowerBarActive) {
                    stopPowerBar();
                } else {
                    startPowerBar();
                }
            }
            // Vérification si le clic est sur le curseur d'effet
            else if (evt.x >= 70 && evt.x <= 270 && evt.y >= 100 && evt.y <= 130) {
                // Mise à jour de l'effet
                float effect = ((evt.x - 70) / 200.0f) * 2.0f - 1.0f;
                setSpinEffect(effect);
            }
        }
    }
}

void AimingSystem::setAimingDirection(const Ogre::Vector3& direction)
{
    mAimingDirection = direction.normalisedCopy();
}

Ogre::Vector3 AimingSystem::getAimingDirection() const
{
    return mAimingDirection;
}

void AimingSystem::startPowerBar(){
    mPowerBarActive = true;
    mPowerValue = MIN_POWER;
    mPowerDirection = 1;
    
    // Affichage de la barre de puissance
    if (powerBarContainer) {
        powerBarContainer->show();
    }
}

void AimingSystem::stopPowerBar()
{
    mPowerBarActive = false;
    
    // La valeur de puissance est conservée pour le lancer
}

float AimingSystem::getPower() const
{
    return mPowerValue;
}

bool AimingSystem::isPowerBarActive() const
{
    return mPowerBarActive;
}

void AimingSystem::setSpinEffect(float effect)
{
    // Limitation de l'effet entre -1.0 et 1.0
    mSpinEffect = std::max(-1.0f, std::min(1.0f, effect));
}

float AimingSystem::getSpinEffect() const
{
    return mSpinEffect;
}

void AimingSystem::resetAiming(){
    // Réinitialisation de la visée
    mAimingDirection = Ogre::Vector3(0, 0, -1);
    
    // Réinitialisation de la puissance
    mPowerBarActive = false;
    mPowerValue = MIN_POWER;
    
    // Masquage de la barre de puissance
    if (powerBarContainer) {
        powerBarContainer->hide();
    }
    
    // Réinitialisation de l'effet
    mSpinEffect = 0.0f;
    
    // Mise à jour des éléments visuels
    updateAimingArrow();
    updateSpinEffectControl();
}

bool AimingSystem::isAimingActive() const
{
    return mAimingActive;
}

void AimingSystem::setAimingActive(bool active){
    mAimingActive = active;
    
    // Affichage ou masquage des éléments visuels
    if (arrowNode) {
        if (active) {
            arrowNode->setVisible(true);
        } else {
            arrowNode->setVisible(false);
        }
    }
}

Ogre::SceneNode* AimingSystem::getArrowNode() const{
    return arrowNode;
}

Ogre::OverlayElement* AimingSystem::getPowerBarElement() const
{
    return mPowerBarFill;
}

Ogre::OverlayElement* AimingSystem::getSpinEffectElement() const
{
    return mSpinEffectIndicator;
}