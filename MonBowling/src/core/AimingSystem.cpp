#include "../../include/core/AimingSystem.h"
#include <OgreMaterialManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreResourceGroupManager.h>
#include <OgreStringConverter.h>
#include <algorithm>
#include <iostream>

AimingSystem::AimingSystem(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera)
    : mSceneMgr(sceneMgr),
      mCamera(camera),
      mArrowNode(nullptr),
      mArrowEntity(nullptr),
      mGameOverlay(nullptr),
      mPowerBarContainer(nullptr),
      mPowerBarBackground(nullptr),
      mPowerBarFill(nullptr),
      mSpinEffectControl(nullptr),
      mSpinEffectIndicator(nullptr),
      mCancelButton(nullptr),
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
    // Nettoyage des overlays
    if (mGameOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(mGameOverlay);
    }
    
    // Nettoyage des nœuds de scène
    if (mArrowNode) {
        mArrowNode->detachAllObjects();
        mSceneMgr->destroySceneNode(mArrowNode);
    }
    
    // Nettoyage des entités
    if (mArrowEntity) {
        mSceneMgr->destroyEntity(mArrowEntity);
    }
}

void AimingSystem::debugForceBar() {
    // Force la visibilité de tous les éléments d'UI
    if (mGameOverlay) {
        mGameOverlay->show();
    }
    
    if (mPowerBarContainer) {
        mPowerBarContainer->show();
        // Position plus évidente
        mPowerBarContainer->setPosition(20, 20);  // Centre-gauche
        mPowerBarContainer->setDimensions(50, 300);  // Plus grand
        
        if (mPowerBarFill) {
            // Forcer une valeur visible pour le remplissage
            mPowerBarFill->setPosition(0, 150);
            mPowerBarFill->setDimensions(50, 150);  // 50% de remplissage
            
            // Couleur éclatante
            Ogre::MaterialPtr fillMaterial = Ogre::MaterialManager::getSingleton().getByName("PowerBarFillMaterial");
            if (fillMaterial) {
                Ogre::Pass* pass = fillMaterial->getTechnique(0)->getPass(0);
                pass->setDiffuse(Ogre::ColourValue(1.0f, 0.0f, 0.0f, 1.0f));  // Rouge vif
            }
        }
    }
/*
    // 2. Créez d'abord votre matériau s'il n'existe pas déjà
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        "PowerBarBackgroundMaterial2", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
    material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);
    material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    
    // Définir une couleur rouge semi-transparente (RGBA)
    material->getTechnique(0)->getPass(0)->setAmbient(1.0, 0.0, 0.0);
    material->getTechnique(0)->getPass(0)->setDiffuse(1.0, 0.0, 0.0, 0.7);
    
    // 3. Accédez au gestionnaire d'overlay
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    // 4. Créez l'overlay (en supprimant d'abord s'il existe déjà)
    Ogre::Overlay* existingOverlay = overlayManager.getByName("TestOverlay");
    if (existingOverlay) {
        overlayManager.destroy("TestOverlay");
    }
    Ogre::Overlay* testOverlay = overlayManager.create("TestOverlay");
    
    // 5. Créez le container (panel)
    Ogre::OverlayContainer* testContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "TestContainer"));
    
    // 6. Configurez le container - utiliser des valeurs relatives peut être plus fiable
    testContainer->setMetricsMode(Ogre::GMM_RELATIVE); // Mode relatif (0-1) plutôt que pixels
    testContainer->setPosition(0.05, 0.05);           // 5% depuis le coin supérieur gauche
    testContainer->setDimensions(0.2, 0.2);          // 20% de la taille de l'écran
    testContainer->setMaterialName("PowerBarBackgroundMaterial2");
    
    // 7. Ajoutez le container à l'overlay
    testOverlay->add2D(testContainer);
    
    // 8. Définissez un Z-order élevé (pour être sûr qu'il est au-dessus)
    testOverlay->setZOrder(655);
    
    // 9. Rendez l'overlay visible
    testOverlay->show();
*/
    
    // Afficher un message dans la console
    std::cout << "Debug force bar called - UI elements should be visible" << std::endl;

    
}

void AimingSystem::initialize(){
    // Création des éléments visuels
    createAimingArrow();
    createPowerBar();
    createSpinEffectControl();
    createCancelButton();
    
    // Activation du système de visée
    setAimingActive(true);
}

void AimingSystem::createAimingArrow(){
    // Création d'une flèche directionnelle simple
    // Utilisation d'un cône pour représenter la flèche
    mArrowEntity = mSceneMgr->createEntity("AimingArrow", "cone.mesh");
    
    // Création d'un matériau pour la flèche
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        "AimingArrowMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* technique = material->getTechnique(0);
    Ogre::Pass* pass = technique->getPass(0);
    
    pass->setDiffuse(Ogre::ColourValue(1.0f, 0.5f, 0.0f, 0.7f));  // Orange semi-transparent
    pass->setSpecular(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
    pass->setShininess(30);
    pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    pass->setDepthWriteEnabled(false);
    
    mArrowEntity->setMaterialName("AimingArrowMaterial");
    
    // Création du nœud pour la flèche
    mArrowNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("AimingArrowNode");
    mArrowNode->attachObject(mArrowEntity);
    
    // Orientation et mise à l'échelle de la flèche
    mArrowNode->setScale(0.5f, 2.0f, 0.5f);
    mArrowNode->pitch(Ogre::Degree(-90));  // Orienter la flèche vers l'avant
    
    // Positionnement initial de la flèche (à ajuster selon la position de la boule)
    mArrowNode->setPosition(14.0f, 1.5f, -69.0f);  // Légèrement au-dessus du sol
}

void AimingSystem::createPowerBar(){
    // Création de l'overlay pour l'interface utilisateur
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    if (!mGameOverlay) {
        mGameOverlay = overlayManager.create("GameOverlay");
    }
    
    // Création du conteneur pour la barre de puissance
    mPowerBarContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "PowerBarContainer"));
    mPowerBarContainer->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarContainer->setPosition(20, 100);
    mPowerBarContainer->setDimensions(30, 200);
    
    // Création de l'arrière-plan de la barre de puissance
    mPowerBarBackground = overlayManager.createOverlayElement("Panel", "PowerBarBackground");
    mPowerBarBackground->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarBackground->setPosition(0, 0);
    mPowerBarBackground->setDimensions(30, 200);
    
    // Création du matériau pour l'arrière-plan
    Ogre::MaterialPtr backgroundMaterial = Ogre::MaterialManager::getSingleton().create(
        "PowerBarBackgroundMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* backgroundTechnique = backgroundMaterial->getTechnique(0);
    Ogre::Pass* backgroundPass = backgroundTechnique->getPass(0);
    
    // backgroundPass->setDiffuse(Ogre::ColourValue(0.2f, 0.2f, 0.2f, 0.8f));
    backgroundPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    backgroundPass->setDepthWriteEnabled(false);
    // backgroundPass->setDiffuse(Ogre::ColourValue(0.5f, 0.5f, 0.5f, 1.0f));  // Plus opaque et plus clair

    mPowerBarBackground->setMaterialName("PowerBarBackgroundMaterial");
    
    // Création de la barre de remplissage
    mPowerBarFill = overlayManager.createOverlayElement("Panel", "PowerBarFill");
    mPowerBarFill->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarFill->setPosition(0, 200);  // Commence en bas
    mPowerBarFill->setDimensions(30, 0);  // Hauteur initiale à 0
    
    // Création du matériau pour la barre de remplissage
    Ogre::MaterialPtr fillMaterial = Ogre::MaterialManager::getSingleton().create(
        "PowerBarFillMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* fillTechnique = fillMaterial->getTechnique(0);
    Ogre::Pass* fillPass = fillTechnique->getPass(0);
        // Rendre la barre plus voyante pour le débogage

    // fillPass->setDiffuse(Ogre::ColourValue(1.0f, 0.0f, 0.0f, 1.0f));  // Rouge vif et opaque

    // backgroundPass->setDiffuse(Ogre::ColourValue(0.0f, 0.0f, 1.0f, 1.0f));  // Bleu opaque
    // fillPass->setDiffuse(Ogre::ColourValue(1.0f, 0.0f, 0.0f, 1.0f));  // Rouge opaque   
    
    // fillPass->setDiffuse(Ogre::ColourValue(0.0f, 1.0f, 0.0f, 0.8f));  // Vert
    fillPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    fillPass->setDepthWriteEnabled(false);
    
    // Assemblage de la barre de puissance
    mPowerBarContainer->addChild(mPowerBarBackground);
    mPowerBarContainer->addChild(mPowerBarFill);

    mPowerBarFill->setMaterialName("PowerBarFillMaterial");
    
    // Ajout à l'overlay principal
    mGameOverlay->add2D(mPowerBarContainer);
    
    // Masquer la barre de puissance initialement
    //mPowerBarContainer->hide();
    mPowerBarContainer->show();

    // Affichage de l'overlay
    mGameOverlay->setZOrder(300);

    mGameOverlay->show();

    std::cout << "GAMEOVERLAY-DEBUT" << std::endl;

    if (mGameOverlay->isVisible()) {
        std::cout << "GameOverlay est visible" << std::endl;
    } else {
        std::cout << "GameOverlay est masqué !" << std::endl;
    }
    
    std::cout << "GAMEOVERLAY-FIN" << std::endl;

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
    
    // Création du matériau pour l'arrière-plan du curseur
    Ogre::MaterialPtr spinBackgroundMaterial = Ogre::MaterialManager::getSingleton().create(
        "SpinEffectBackgroundMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* spinBackgroundTechnique = spinBackgroundMaterial->getTechnique(0);
    Ogre::Pass* spinBackgroundPass = spinBackgroundTechnique->getPass(0);
    
    spinBackgroundPass->setDiffuse(Ogre::ColourValue(0.2f, 0.2f, 0.2f, 0.8f));
    spinBackgroundPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    spinBackgroundPass->setDepthWriteEnabled(false);

    mSpinEffectControl->setMaterialName("SpinEffectBackgroundMaterial");
    
    // Création de l'indicateur du curseur d'effet
    mSpinEffectIndicator = overlayManager.createOverlayElement("Panel", "SpinEffectIndicator");
    mSpinEffectIndicator->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinEffectIndicator->setPosition(95, 0);  // Position centrale
    mSpinEffectIndicator->setDimensions(10, 30);

    // Création du matériau pour l'indicateur du curseur
    Ogre::MaterialPtr spinIndicatorMaterial = Ogre::MaterialManager::getSingleton().create(
        "SpinEffectIndicatorMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* spinIndicatorTechnique = spinIndicatorMaterial->getTechnique(0);
    Ogre::Pass* spinIndicatorPass = spinIndicatorTechnique->getPass(0);
    
    spinIndicatorPass->setDiffuse(Ogre::ColourValue(1.0f, 0.0f, 0.0f, 0.8f));  // Rouge
    spinIndicatorPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    spinIndicatorPass->setDepthWriteEnabled(false);

    mSpinEffectIndicator->setMaterialName("SpinEffectIndicatorMaterial");
    
    // Assemblage du curseur d'effet
    spinContainer->addChild(mSpinEffectControl);
    spinContainer->addChild(mSpinEffectIndicator);
    
    // Ajout à l'overlay principal
    mGameOverlay->add2D(spinContainer);
}

void AimingSystem::createCancelButton(){
    // Création du bouton d'annulation
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    // Création du conteneur pour le bouton
    Ogre::OverlayContainer* cancelContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "CancelButtonContainer"));
    cancelContainer->setMetricsMode(Ogre::GMM_PIXELS);
    cancelContainer->setPosition(20, 320);
    cancelContainer->setDimensions(100, 40);
    
    // Création du bouton
    mCancelButton = overlayManager.createOverlayElement("Panel", "CancelButton");
    mCancelButton->setMetricsMode(Ogre::GMM_PIXELS);
    mCancelButton->setPosition(0, 0);
    mCancelButton->setDimensions(100, 40);
    
    // Création du matériau pour le bouton
    Ogre::MaterialPtr cancelButtonMaterial = Ogre::MaterialManager::getSingleton().create(
        "CancelButtonMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    // Obtention de la technique et du pass
    Ogre::Technique* cancelButtonTechnique = cancelButtonMaterial->getTechnique(0);
    Ogre::Pass* cancelButtonPass = cancelButtonTechnique->getPass(0);
    
    cancelButtonPass->setDiffuse(Ogre::ColourValue(0.8f, 0.0f, 0.0f, 0.8f));  // Rouge
    cancelButtonPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    cancelButtonPass->setDepthWriteEnabled(false);

    mCancelButton->setMaterialName("CancelButtonMaterial");
    
    // Création du texte pour le bouton
    Ogre::TextAreaOverlayElement* cancelText = static_cast<Ogre::TextAreaOverlayElement*>(
        overlayManager.createOverlayElement("TextArea", "CancelButtonText"));
    cancelText->setMetricsMode(Ogre::GMM_PIXELS);
    cancelText->setPosition(20, 10);
    cancelText->setDimensions(60, 20);
    cancelText->setCharHeight(16);
    cancelText->setFontName("Arial");
    cancelText->setColour(Ogre::ColourValue::White);
    cancelText->setCaption("Annuler");
    
    // Assemblage du bouton
    cancelContainer->addChild(mCancelButton);
    cancelContainer->addChild(cancelText);
    
    // Ajout à l'overlay principal
    mGameOverlay->add2D(cancelContainer);
    
    // Création du conteneur pour l'affichage du score
    mScoreContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "ScoreContainer"));
    mScoreContainer->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreContainer->setPosition(20, 20);
    mScoreContainer->setDimensions(100, 30);
    
    // Création de l'affichage du score
    mScoreDisplay = static_cast<Ogre::TextAreaOverlayElement*>(
        overlayManager.createOverlayElement("TextArea", "ScoreDisplay"));
    mScoreDisplay->setMetricsMode(Ogre::GMM_PIXELS);
    mScoreDisplay->setPosition(0, 0);
    mScoreDisplay->setDimensions(100, 30);
    mScoreDisplay->setCharHeight(24);
    mScoreDisplay->setFontName("Arial");
    mScoreDisplay->setColour(Ogre::ColourValue::White);
    mScoreDisplay->setCaption("Score: 0");
    
    // Ajout du texte au conteneur
    mScoreContainer->addChild(mScoreDisplay);
    
    // Ajout du conteneur à l'overlay principal
    mGameOverlay->add2D(mScoreContainer);
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
    // Mise à jour de la position et de l'orientation de la flèche
    if (mArrowNode) {
        // La flèche pointe dans la direction de visée
        Ogre::Vector3 direction = mAimingDirection.normalisedCopy();
        
        // Calcul de la rotation pour orienter la flèche
        Ogre::Quaternion rotation = Ogre::Vector3::UNIT_Z.getRotationTo(direction);
        mArrowNode->setOrientation(rotation);
        
        // La flèche reste à une position fixe (à ajuster selon la position de la boule)
        // mArrowNode->setPosition(...);
    }
}

void AimingSystem::updatePowerBar(float deltaTime){
    // Mise à jour de la barre de puissance
    if (mPowerBarActive && mPowerBarFill) {
        // Mise à jour de la valeur de puissance
        mPowerValue += mPowerDirection * mPowerSpeed * deltaTime;
        
        // Inversion de la direction si les limites sont atteintes
        if (mPowerValue >= MAX_POWER) {
            mPowerValue = MAX_POWER;
            mPowerDirection = -1;
        } else if (mPowerValue <= MIN_POWER) {
            mPowerValue = MIN_POWER;
            mPowerDirection = 1;
        }
        
        // Mise à jour de l'affichage de la barre
        float fillHeight = (mPowerValue / MAX_POWER) * 200.0f;
        mPowerBarFill->setPosition(0, 200.0f - fillHeight);
        mPowerBarFill->setDimensions(30, fillHeight);
        
        // Changement de couleur en fonction de la puissance
        Ogre::MaterialPtr fillMaterial = Ogre::MaterialManager::getSingleton().getByName("PowerBarFillMaterial");
        if (fillMaterial) {
            // Obtention de la technique et du pass
            Ogre::Pass* pass = fillMaterial->getTechnique(0)->getPass(0);
            
            // Transition de vert à rouge en fonction de la puissance
            float ratio = mPowerValue / MAX_POWER;
            pass->setDiffuse(Ogre::ColourValue(ratio, 1.0f - ratio, 0.0f, 0.8f));
        }
    }
}

void AimingSystem::updateSpinEffectControl(){
    // Mise à jour de la position de l'indicateur d'effet
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
        Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
            evt.x / (float)mCamera->getViewport()->getActualWidth(),
            evt.y / (float)mCamera->getViewport()->getActualHeight());
        
        // Intersection avec le plan du sol (y = 0)
        Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0);
        std::pair<bool, Ogre::Real> intersection = mouseRay.intersects(groundPlane);
        
        if (intersection.first) {
            // Point d'intersection avec le sol
            Ogre::Vector3 hitPoint = mouseRay.getPoint(intersection.second);
            
            // Calcul de la direction de visée (depuis la position de la boule vers le point d'intersection)
            Ogre::Vector3 ballPosition(14.0f, 1.5f, -69.0f);  // À ajuster selon la position réelle de la boule
            Ogre::Vector3 direction = (hitPoint - ballPosition).normalisedCopy();
            
            // Mise à jour de la direction de visée
            setAimingDirection(direction);
        }
        
        // Gestion du curseur d'effet
    }
}

void AimingSystem::setAimingDirection(const Ogre::Vector3& direction) {
    mAimingDirection = direction;
}

void AimingSystem::setAimingActive(bool active) {
    mAimingActive = active;
}

Ogre::Vector3 AimingSystem::getAimingDirection() const {
    return mAimingDirection;
}

float AimingSystem::getPower() const {
    return mPowerValue;
}

float AimingSystem::getSpinEffect() const {
    return mSpinEffect;
}

void AimingSystem::resetAiming(){
    mAimingDirection = Ogre::Vector3::ZERO;  
    mPowerValue = 0.0f;                      
    mSpinEffect = 0.0f;                       
    mAimingActive = false;
    
    if (mPowerBarContainer) {
        mPowerBarContainer->hide();
    }
    
    if (mArrowNode) {
        mArrowNode->setVisible(false);
    }
    
    if (mArrowNode) {
        mArrowNode->setOrientation(Ogre::Quaternion::IDENTITY);
        mArrowNode->setPosition(Ogre::Vector3::ZERO);
        mArrowNode->setVisible(true); 
    }
    
    if (mPowerBarContainer) {
        mPowerBarContainer->setPosition(400, 200);
        mPowerBarContainer->setDimensions(50, 300);
        mPowerBarContainer->show();
    }
    
    updateAimingArrow();
}

void AimingSystem::startPowerBar() {
    mPowerBarActive = true;
    if (mGameOverlay) {
        mGameOverlay->show();  // S'assurer que l'overlay principal est visible
    }
    mPowerBarContainer->show();
    std::cout << "startPowerBar appelé, mPowerBarActive = " << mPowerBarActive << std::endl;    
    std::cout << "PowerBar position: " << mPowerBarContainer->getLeft() 
              << ", " << mPowerBarContainer->getTop() << std::endl;
}

void AimingSystem::showAimingArrow() {
    if (mArrowNode) {
        mArrowNode->setVisible(true);
    }
}

void AimingSystem::hideAimingArrow() {
    if (mArrowNode) {
        mArrowNode->setVisible(false);
    }
}

void AimingSystem::stopPowerBar() {
    mPowerBarActive = false;
    mPowerBarContainer->hide();
}

void AimingSystem::hidePowerBar() {
    mPowerBarActive = false;
    mPowerBarContainer->hide();
}

