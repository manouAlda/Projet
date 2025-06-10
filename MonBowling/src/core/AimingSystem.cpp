// Fichier : AimingSystem.cpp
#include "../../include/core/AimingSystem.h"
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreColourValue.h>
#include <algorithm> // Pour std::max et std::min

AimingSystem::AimingSystem(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera)
    : scene(sceneMgr),
      camera(camera),
      arrowNode(nullptr),
      arrowEntity(nullptr),
      gameOverlay(nullptr),
      powerBarContainer(nullptr),
      mPowerBarBackground(nullptr),
      mPowerBarFill(nullptr),
      mSpinContainer(nullptr),
      mSpinEffectControl(nullptr),
      mSpinEffectIndicator(nullptr),
      mAimingActive(false),
      mPowerInputActive(false),
      mAimingDirection(0, 0, -1), 
      mPowerValue(MIN_POWER),
      mSpinEffect(0.0f),
      mMaxSpinRate(2.0f),      // vitesse de changement
      mSpinSensitivity(0.05f), // Sensibilité du contrôle de spin
      mShowSpinPreview(true)   // Option pour afficher un preview de la trajectoire
{
}

AimingSystem::~AimingSystem(){
    // La destruction de l'overlay et des éléments est gérée par Ogre
    // si l'overlay est détruit.
    if (gameOverlay) {
        Ogre::OverlayManager::getSingleton().destroy(gameOverlay);
        gameOverlay = nullptr; // Bonne pratique
    }

    // Nettoyage des éléments de scène
    if (arrowNode) {
        arrowNode->detachAllObjects();
        scene->destroySceneNode(arrowNode);
        arrowNode = nullptr;
    }
    if (arrowEntity) {
        scene->destroyEntity(arrowEntity);
        arrowEntity = nullptr;
    }
}

void AimingSystem::initialize(){
    createAimingArrow();
    createOverlays(); 

    // Initialisation de l'état
    resetAiming();
    setAimingActive(true);
    Ogre::LogManager::getSingleton().logMessage("AimingSystem initialisé.");
}

void AimingSystem::createAimingArrow(){
    try {
        arrowEntity = scene->createEntity("AimingArrow", "cone.mesh");
        // Assigner un matériau visible, s'assurer qu'il existe
        arrowEntity->setMaterialName("BaseWhite"); // Utiliser un matériau simple pour tester

        arrowNode = scene->getRootSceneNode()->createChildSceneNode("AimingArrowNode");
        arrowNode->attachObject(arrowEntity);

        // Ajuster l'échelle et l'orientation
        arrowNode->setScale(0.05f, 0.2f, 0.05f);
        arrowNode->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)); 

        // Position initiale (devant la position de départ de la boule)
        arrowNode->setPosition(0.0f, 0.1f, -7.0f);
        arrowNode->setVisible(false); // Invisible au début
    } catch (Ogre::Exception& e) {
        Ogre::LogManager::getSingleton().logError("AimingSystem::createAimingArrow - Erreur: " + Ogre::String(e.getFullDescription().c_str()));
        if(arrowNode) scene->destroySceneNode(arrowNode);
        if(arrowEntity) scene->destroyEntity(arrowEntity);
        arrowNode = nullptr;
        arrowEntity = nullptr;
    }
}

void AimingSystem::createOverlays(){
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    gameOverlay = overlayManager.create("GameOverlay");
    gameOverlay->setZOrder(500); // Mettre au premier plan

    // --- Barre de Puissance --- 
    powerBarContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "PowerBarContainer"));
    powerBarContainer->setMetricsMode(Ogre::GMM_PIXELS);
    powerBarContainer->setPosition(20, 50); // Position ajustée
    powerBarContainer->setDimensions(30, 200);

    mPowerBarBackground = overlayManager.createOverlayElement("Panel", "PowerBarBackground");
    mPowerBarBackground->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarBackground->setPosition(0, 0);
    mPowerBarBackground->setDimensions(30, 200);
    mPowerBarBackground->setMaterialName("UI/OverlayBackground"); // Matériau simple gris/blanc
    // Pour un fond plus distinct : mPowerBarBackground->setColour(Ogre::ColourValue(0.3, 0.3, 0.3, 0.7));
    powerBarContainer->addChild(mPowerBarBackground);

    // !! Création de l'élément de remplissage !!
    mPowerBarFill = overlayManager.createOverlayElement("Panel", "PowerBarFill");
    mPowerBarFill->setMetricsMode(Ogre::GMM_PIXELS);
    mPowerBarFill->setPosition(0, 200); // Commence en bas
    mPowerBarFill->setDimensions(30, 0); // Hauteur initiale nulle
    mPowerBarFill->setMaterialName("UI/PowerBarFill"); // Matériau simple, la couleur sera changée
    powerBarContainer->addChild(mPowerBarFill); // Ajouter au conteneur

    gameOverlay->add2D(powerBarContainer);
    powerBarContainer->hide(); // Caché initialement

    // --- Contrôle de Spin --- 
    mSpinContainer = static_cast<Ogre::OverlayContainer*>(
        overlayManager.createOverlayElement("Panel", "SpinEffectContainer"));
    mSpinContainer->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinContainer->setPosition(70, 50); // À côté de la barre de puissance
    mSpinContainer->setDimensions(200, 30);

    mSpinEffectControl = overlayManager.createOverlayElement("Panel", "SpinEffectControl");
    mSpinEffectControl->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinEffectControl->setPosition(0, 0);
    mSpinEffectControl->setDimensions(200, 30);
    mSpinEffectControl->setMaterialName("UI/SpinControlBackground");
    // Pour un fond plus distinct : 
    mSpinEffectControl->setColour(Ogre::ColourValue(0.3, 0.3, 0.3, 0.7));
    mSpinContainer->addChild(mSpinEffectControl);

    // !! Création de l'indicateur de spin !!
    mSpinEffectIndicator = overlayManager.createOverlayElement("Panel", "SpinEffectIndicator");
    mSpinEffectIndicator->setMetricsMode(Ogre::GMM_PIXELS);
    mSpinEffectIndicator->setDimensions(10, 30); // Largeur de l'indicateur
    mSpinEffectIndicator->setMaterialName("UI/SpinIndicator"); // Matériau simple
    mSpinEffectIndicator->setColour(Ogre::ColourValue::Red); // Couleur visible
    mSpinContainer->addChild(mSpinEffectIndicator); // Ajouter au conteneur

    gameOverlay->add2D(mSpinContainer);
    mSpinContainer->hide(); // Caché initialement

    // Afficher l'overlay global
    gameOverlay->show();

    // Mettre à jour l'affichage initial (barres vides)
    updatePowerBarDisplay();
    updateSpinIndicatorDisplay();
}

void AimingSystem::update(float deltaTime){
    // Met à jour la flèche si la visée est active
    if (mAimingActive) {
        updateAimingArrow();
    }
    if (mPowerInputActive) {
        updatePowerBarDisplay();
        updateSpinIndicatorDisplay();
        if (mShowSpinPreview) {
            updateTrajectoryPreview();
        }
    }
}

void AimingSystem::updateAimingArrow(){
    if (arrowNode && arrowNode->getAttachedObject(0)) { // Vérifier si l'entité est attachée
        // Rotation pour pointer dans la direction mAimingDirection
        // La flèche (cone.mesh) pointe naturellement vers Y+ local.
        // On veut qu'elle pointe vers mAimingDirection.
        Ogre::Vector3 src = Ogre::Vector3::UNIT_Y;
        Ogre::Quaternion rotation = src.getRotationTo(mAimingDirection);
        arrowNode->setOrientation(rotation);
    } else if (arrowNode) {
         // Si l'entité n'est pas là, on ne fait rien ou on log une erreur
         // Ogre::LogManager::getSingleton().logMessage("AimingSystem::updateAimingArrow - Arrow entity not attached.");
    }
}

void AimingSystem::updatePowerBarDisplay() {
    if (mPowerBarFill) {
        float fillHeight = (mPowerValue / MAX_POWER) * 200.0f; // 200 est la hauteur du conteneur
        fillHeight = std::max(0.0f, std::min(fillHeight, 200.0f)); // Clamp

        mPowerBarFill->setPosition(0, 200.0f - fillHeight); // Position Y (depuis le haut)
        mPowerBarFill->setDimensions(30, fillHeight); // Largeur fixe, hauteur variable

        // Définir la couleur jaune
        Ogre::ColourValue Yellow(1.0f, 1.0f, 0.0f, 1.0f);

        // Changement de couleur en fonction de la puissance (vert -> jaune -> rouge)
        float ratio = mPowerValue / MAX_POWER;
        Ogre::ColourValue color;
        if (ratio < 0.5f) {
            color = Ogre::ColourValue(
                Ogre::Math::lerp(Ogre::ColourValue::Green.r, Yellow.r, ratio * 2.0f),
                Ogre::Math::lerp(Ogre::ColourValue::Green.g, Yellow.g, ratio * 2.0f),
                Ogre::Math::lerp(Ogre::ColourValue::Green.b, Yellow.b, ratio * 2.0f),
                Ogre::Math::lerp(Ogre::ColourValue::Green.a, Yellow.a, ratio * 2.0f)
            );
        } else {
            color = Ogre::ColourValue(
                Ogre::Math::lerp(Yellow.r, Ogre::ColourValue::Red.r, (ratio - 0.5f) * 2.0f),
                Ogre::Math::lerp(Yellow.g, Ogre::ColourValue::Red.g, (ratio - 0.5f) * 2.0f),
                Ogre::Math::lerp(Yellow.b, Ogre::ColourValue::Red.b, (ratio - 0.5f) * 2.0f),
                Ogre::Math::lerp(Yellow.a, Ogre::ColourValue::Red.a, (ratio - 0.5f) * 2.0f)
            );
        }
        mPowerBarFill->setColour(color);
        Ogre::LogManager::getSingleton().logMessage("Power Bar Updated: Value=" + Ogre::StringConverter::toString(mPowerValue) + " Height=" + Ogre::StringConverter::toString(fillHeight));
    }
}

void AimingSystem::updateSpinIndicatorDisplay() {
    if (mSpinEffectIndicator) {
        float controlWidth = 200.0f;
        float indicatorWidth = 10.0f;
        float travelRange = controlWidth - indicatorWidth;

        // Position basée sur la valeur de spin
        float positionX = ((mSpinEffect - MIN_SPIN) / (MAX_SPIN - MIN_SPIN)) * travelRange;
        positionX = std::max(0.0f, std::min(positionX, travelRange));

        mSpinEffectIndicator->setPosition(positionX, 0);
        
        // AMÉLIORATION: Changer la couleur selon l'intensité du spin
        float spinIntensity = std::abs(mSpinEffect) / MAX_SPIN;
        Ogre::ColourValue color;
        
        if (mSpinEffect > 0) {
            // Spin positif = nuances de rouge/orange
            color = Ogre::ColourValue(1.0f, 1.0f - spinIntensity * 0.5f, 0.0f, 1.0f);
        } else if (mSpinEffect < 0) {
            // Spin négatif = nuances de bleu
            color = Ogre::ColourValue(0.0f, spinIntensity * 0.5f, 1.0f, 1.0f);
        } else {
            // Pas de spin = blanc
            color = Ogre::ColourValue::White;
        }
        
        mSpinEffectIndicator->setColour(color);
        
        // Effet de pulsation pour indiquer l'intensité
        float pulseScale = 1.0f + spinIntensity * 0.3f;
        mSpinEffectIndicator->setDimensions(indicatorWidth * pulseScale, 30);
    }
}

void AimingSystem::updateTrajectoryPreview() {
    // Cette méthode créerait une ligne pointillée montrant la trajectoire prévue
    // en tenant compte du spin (optionnel, pour une expérience utilisateur avancée)
    
    if (!mShowSpinPreview || !arrowNode) return;
    
    // Calculer la courbure approximative basée sur le spin
    float curvature = mSpinEffect * 0.2f; // Facteur de courbure
    
    // Créer ou mettre à jour une ligne de trajectoire courbée
    // (Implémentation dépendante de votre système de rendu)
    Ogre::LogManager::getSingleton().logMessage(
        "Trajectory preview: Spin=" + Ogre::StringConverter::toString(mSpinEffect) + 
        " Curvature=" + Ogre::StringConverter::toString(curvature));
}

// --- Gestion des entrées --- 

void AimingSystem::handleKeyPress(const OgreBites::KeyboardEvent& evt) {
    if (!mPowerInputActive) return;

    float powerDelta = 0.0f;
    float spinDelta = 0.0f;

    // Facteur de vitesse basé sur le temps (pour un contrôle plus fluide)
    float speedFactor = 1.0f; // À multiplier par deltaTime dans update()

    switch (evt.keysym.sym) {
        case OgreBites::SDLK_UP:
        case 'z': 
        case 'w': 
            powerDelta = POWER_INCREMENT * 0.15f;
            break;
        case OgreBites::SDLK_DOWN:
        case 's':
            powerDelta = -POWER_INCREMENT * 0.15f;
            break;
        case OgreBites::SDLK_LEFT:
        case 'q': 
        case 'a':
            spinDelta = -SPIN_INCREMENT * mSpinSensitivity;
            break;
        case OgreBites::SDLK_RIGHT:
        case 'd':
            spinDelta = SPIN_INCREMENT * mSpinSensitivity;
            break;
        // NOUVELLES TOUCHES pour un meilleur contrôle
        case 'e': // Spin rapide à droite
            spinDelta = SPIN_INCREMENT * 0.2f;
            break;
        case 'x': // Reset du spin à zéro
            mSpinEffect = 0.0f;
            updateSpinIndicatorDisplay();
            Ogre::LogManager::getSingleton().logMessage("AimingSystem: Spin reset to zero");
            return;
        default:
            return; 
    }

    if (powerDelta != 0.0f) {
        adjustPower(powerDelta);
    }
    if (spinDelta != 0.0f) {
        adjustSpinSmooth(spinDelta);
    }
}

void AimingSystem::handleKeyRelease(const OgreBites::KeyboardEvent& evt) {
    if (!mPowerInputActive) return;

    // Déclencher le lancer SEULEMENT si on relâche la touche HAUT
    if (evt.keysym.sym == OgreBites::SDLK_UP || evt.keysym.sym == 'z' || evt.keysym.sym == 'w') {
        // Le GameManager devrait être notifié ici pour changer d'état et lancer la boule
        // On désactive l'input ici pour éviter des lancements multiples
        mPowerInputActive = false;
        Ogre::LogManager::getSingleton().logMessage("AimingSystem: Power key released. Ready to launch.");
        // GameManager::getInstance()->requestLaunch(); // Exemple d'appel (à implémenter dans GameManager)
    }
}

// --- Contrôle de l'état --- 

void AimingSystem::setAimingActive(bool active){
    mAimingActive = active;
    if (arrowNode) {
        arrowNode->setVisible(active);
    }
    // Ne pas affecter mPowerInputActive ici
    Ogre::LogManager::getSingleton().logMessage("AimingSystem: Aiming Active = " + Ogre::StringConverter::toString(active));
}

bool AimingSystem::isAimingActive() const {
    return mAimingActive;
}

// Appelé par GameManager quand on entre dans l'état POWER
void AimingSystem::startPowerPhase() {
    mPowerInputActive = true;
    // Afficher les overlays de puissance et spin
    if (powerBarContainer) powerBarContainer->show();
    if (mSpinContainer) mSpinContainer->show();
    // Réinitialiser les valeurs au début de la phase POWER
    mPowerValue = MIN_POWER;
    mSpinEffect = 0.0f;
    updatePowerBarDisplay();
    updateSpinIndicatorDisplay();
    Ogre::LogManager::getSingleton().logMessage("AimingSystem: Power Phase Started.");
}

void AimingSystem::resetAiming(){
    mAimingActive = false; // Désactiver la visée par défaut au reset
    mPowerInputActive = false;

    mAimingDirection = Ogre::Vector3(0, 0, -1);
    mPowerValue = MIN_POWER;
    mSpinEffect = 0.0f;

    // Cacher les éléments visuels
    if (arrowNode) arrowNode->setVisible(false);
    if (powerBarContainer) powerBarContainer->hide();
    if (mSpinContainer) mSpinContainer->hide();

    // Mettre à jour l'affichage (même si caché, pour la cohérence)
    updatePowerBarDisplay();
    updateSpinIndicatorDisplay();
    Ogre::LogManager::getSingleton().logMessage("AimingSystem: Reset.");
}

SpinDebugInfo AimingSystem::getSpinDebugInfo() const {
    SpinDebugInfo info;
    info.rawSpinValue = mSpinEffect;
    info.normalizedSpin = mSpinEffect / MAX_SPIN;
    info.finalAngularVelocity = getSpinEffect();
    info.isSpinActive = std::abs(mSpinEffect) > 0.01f;
    return info;
}

// --- Getters pour le lancement --- 

Ogre::Vector3 AimingSystem::getAimingDirection() const {
    return mAimingDirection;
}

float AimingSystem::getPower() const {
    // Retourne la puissance finale déterminée par le joueur
    // !! IMPORTANT: Convertir la valeur 0-100 en une force/vitesse pour la physique !!
    // Exemple simple : return mPowerValue * 0.2f; // Ajuster ce facteur
    //return 5.0f + (mPowerValue / MAX_POWER) * 15.0f; // Donne une vitesse entre 5 et 20
    return 10.0f + (mPowerValue / MAX_POWER) * 30.0f;
}

float AimingSystem::getSpinEffect() const {
    // Mapping non-linéaire pour un contrôle plus intuitif
    float normalizedSpin = mSpinEffect / MAX_SPIN; // -1 à 1
    
    // Application d'une courbe pour que les petites valeurs soient plus sensibles
    float curve = std::pow(std::abs(normalizedSpin), 0.7f);
    if (normalizedSpin < 0) curve = -curve;
    
    // Conversion en vitesse angulaire réaliste (rad/s)
    return curve * 12.0f; // Max ~12 rad/s, ce qui est réaliste pour une boule de bowling
}

// --- Méthodes d'ajustement --- 

void AimingSystem::adjustPower(float delta) {
    if (!mPowerInputActive) return;
    mPowerValue += delta;
    // Clamp la valeur
    mPowerValue = std::max(MIN_POWER, std::min(MAX_POWER, mPowerValue));
    updatePowerBarDisplay(); 
}

void AimingSystem::adjustSpin(float delta) {
    if (!mPowerInputActive) return;
    mSpinEffect += delta;
    // Clamp la valeur
    mSpinEffect = std::max(MIN_SPIN, std::min(MAX_SPIN, mSpinEffect));
    updateSpinIndicatorDisplay(); 
}

void AimingSystem::adjustSpinSmooth(float delta) {
    if (!mPowerInputActive) return;
    
    // Application progressive du spin
    float targetSpin = mSpinEffect + delta;
    targetSpin = std::max(MIN_SPIN, std::min(MAX_SPIN, targetSpin));
    
    // Interpolation pour un mouvement plus fluide
    float lerpFactor = 0.1f; // Ajustable pour plus ou moins de fluidité
    mSpinEffect = Ogre::Math::lerp(mSpinEffect, targetSpin, lerpFactor);
    
    updateSpinIndicatorDisplay();
    
    // Mettre à jour le preview de trajectoire si activé
    if (mShowSpinPreview) {
        updateTrajectoryPreview();
    }
}

