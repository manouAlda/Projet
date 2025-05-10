#include "../../Header/Bullet/DebugDrawer.hpp"
#include <iostream>

namespace Bullet {

DebugDrawer::DebugDrawer(Ogre::SceneNode* node, btDynamicsWorld* world) 
    : mNode(node), mWorld(world), mDebugMode(DBG_DrawWireframe) {
    
    // Créer un objet manuel pour dessiner les lignes
    Ogre::SceneManager* sceneMgr = mNode->getCreator();
    mLines = sceneMgr->createManualObject("PhysicsDebugLines");
    mLines->setDynamic(true);
    mNode->attachObject(mLines);
    
    // Créer un matériau pour les lignes
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        "PhysicsDebugMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    material->setReceiveShadows(false);
    material->getTechnique(0)->setLightingEnabled(false);
    material->getTechnique(0)->getPass(0)->setDiffuse(1, 0, 0, 1);
    material->getTechnique(0)->getPass(0)->setAmbient(1, 0, 0);
    material->getTechnique(0)->getPass(0)->setSelfIllumination(1, 0, 0);
    
    // Associer le débogueur au monde
    if (mWorld) {
        mWorld->setDebugDrawer(this);
    }
}

DebugDrawer::~DebugDrawer() {
    // Dissocier le débogueur du monde
    if (mWorld) {
        mWorld->setDebugDrawer(nullptr);
    }
    
    // Le SceneManager s'occupera de nettoyer mLines
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    // Cette méthode est appelée par Bullet pour dessiner des lignes de debug
    // Mais nous ne faisons rien ici car nous allons dessiner toutes les lignes dans update()
}

void DebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, 
                                btScalar distance, int lifeTime, const btVector3& color) {
    // Méthode requise mais pas implémentée pour l'instant
}

void DebugDrawer::reportErrorWarning(const char* warningString) {
    std::cout << "Bullet warning: " << warningString << std::endl;
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString) {
    // Méthode requise mais pas implémentée pour l'instant
}

void DebugDrawer::setDebugMode(int debugMode) {
    mDebugMode = debugMode;
}

int DebugDrawer::getDebugMode() const {
    return mDebugMode;
}

void DebugDrawer::update() {
    // Vérifier si le monde existe
    if (!mWorld) {
        return;
    }
    
    // Effacer les anciennes lignes
    mLines->clear();
    
    // Commencer à dessiner de nouvelles lignes
    mLines->begin("PhysicsDebugMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    
    // Demander à Bullet de dessiner le monde
    mWorld->debugDrawWorld();
    
    // Pour capturer les lignes dessinées par Bullet, nous devrions implémenter
    // notre propre méthode de rendu qui stocke les lignes, mais pour simplifier
    // nous allons simplement dessiner une grille de debug ici
    
    // Dessiner une grille simple pour représenter le plan XZ
    float gridSize = 20.0f;
    float step = 1.0f;
    
    for (float i = -gridSize; i <= gridSize; i += step) {
        // Lignes selon l'axe X
        mLines->position(i, 0, -gridSize);
        mLines->colour(0.5f, 0.5f, 0.5f);
        mLines->position(i, 0, gridSize);
        mLines->colour(0.5f, 0.5f, 0.5f);
        
        // Lignes selon l'axe Z
        mLines->position(-gridSize, 0, i);
        mLines->colour(0.5f, 0.5f, 0.5f);
        mLines->position(gridSize, 0, i);
        mLines->colour(0.5f, 0.5f, 0.5f);
    }
    
    // Axes principaux
    // Axe X (rouge)
    mLines->position(0, 0, 0);
    mLines->colour(1, 0, 0);
    mLines->position(gridSize, 0, 0);
    mLines->colour(1, 0, 0);
    
    // Axe Y (vert)
    mLines->position(0, 0, 0);
    mLines->colour(0, 1, 0);
    mLines->position(0, gridSize, 0);
    mLines->colour(0, 1, 0);
    
    // Axe Z (bleu)
    mLines->position(0, 0, 0);
    mLines->colour(0, 0, 1);
    mLines->position(0, 0, gridSize);
    mLines->colour(0, 0, 1);
    
    // Terminer le dessin
    mLines->end();
}

} // namespace Bullet