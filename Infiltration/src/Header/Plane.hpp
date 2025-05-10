#ifndef PLANE_HPP
#define PLANE_HPP

#include <Ogre.h>

// Forward declarations pour éviter d'inclure tous les headers de Bullet
class btDiscreteDynamicsWorld;
class btRigidBody;
class btCollisionShape;

/**
 * Classe représentant un plan dans la scène
 * Utilisé notamment pour créer le sol ou d'autres surfaces planes
 * Intègre la gestion du corps rigide pour Bullet Physics
 */
class Plane {
public:
    /**
     * Constructeur qui crée un plan pour le sol de la scène
     * @param sceneMgr Le gestionnaire de scène Ogre
     * @param materialName Le nom du matériau à appliquer au plan
     * @param dynamicsWorld Le monde dynamique de Bullet (optionnel)
     */
    Plane(Ogre::SceneManager* sceneMgr, const Ogre::String& materialName, btDiscreteDynamicsWorld* dynamicsWorld = nullptr);
    
    /**
     * Destructeur
     */
    ~Plane();
    
    /**
     * Récupère l'entité du plan
     * @return L'entité du plan
     */
    Ogre::Entity* getEntity() const;
    
    /**
     * Récupère le noeud de scène du plan
     * @return Le noeud de scène du plan
     */
    Ogre::SceneNode* getSceneNode() const;
    
    /**
     * Définit la position du plan
     * @param position La nouvelle position
     */
    void setPosition(const Ogre::Vector3& position);
    
    /**
     * Définit la rotation du plan
     * @param rotation La rotation à appliquer en quaternion
     */
    void setRotation(const Ogre::Quaternion& rotation);
    
    /**
     * Définit le matériau du plan
     * @param materialName Le nom du matériau à appliquer
     */
    void setMaterial(const Ogre::String& materialName);
    
    /**
     * Récupère le corps rigide associé au plan
     * @return Le corps rigide Bullet
     */
    btRigidBody* getRigidBody() const { return mRigidBody; }
    
    /**
     * Définit les propriétés physiques du plan
     * @param friction Coefficient de friction
     * @param restitution Coefficient de restitution (rebond)
     */
    void setPhysicsProperties(float friction, float restitution);
    
private:
    Ogre::SceneManager* mSceneMgr;       // Gestionnaire de scène
    Ogre::Entity* mPlaneEntity;          // Entité du plan
    Ogre::SceneNode* mPlaneNode;         // Noeud de scène du plan
    
    // Éléments de physique
    btDiscreteDynamicsWorld* mDynamicsWorld;  // Monde physique
    btRigidBody* mRigidBody;                  // Corps rigide
    btCollisionShape* mCollisionShape;        // Forme de collision
};

#endif // PLANE_HPP