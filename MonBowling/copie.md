    // Création de la caméra
    cameraNode = scene->getRootSceneNode()->createChildSceneNode("MainCameraNode"); // Nom plus spécifique
    camera = scene->createCamera("MainCamera");
    cameraNode->attachObject(camera);

    // >>> Début de la modification <<<
    // Définir explicitement la position et l'orientation initiales ici.
    Ogre::Vector3 initialCamPos(0.0f, 3.0f, -16.5f); // Correspond à CameraFollower::initialPosition
    Ogre::Vector3 initialLookAt(0.0f, 0.5f, -10.0f); // Correspond à CameraFollower::pinsLookAt (vers les quilles)

    cameraNode->setPosition(initialCamPos);

    // Calculer l'orientation manuellement pour contrôler l'axe "up"
    Ogre::Vector3 direction = initialLookAt - initialCamPos;
    // Assumons que l'axe Y du monde est bien l'axe "haut" visuel
    Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y;

    // Créer le quaternion d'orientation en calculant les axes
    // (Assure que les axes sont orthogonaux)
    Ogre::Vector3 zAxis = direction.normalisedCopy(); // Direction de vue
    Ogre::Vector3 xAxis = upVector.crossProduct(zAxis).normalisedCopy(); // Axe droite
    Ogre::Vector3 yAxis = zAxis.crossProduct(xAxis).normalisedCopy(); // Axe haut (recalculé pour orthogonalité)

    Ogre::Quaternion orientation;
    orientation.FromAxes(xAxis, yAxis, zAxis);
    cameraNode->setOrientation(orientation);
    // >>> Fin de la modification <<<

    camera->setNearClipDistance(1);

    // Configuration du viewport
    // ... (le reste de la fonction setup)
    ********************************************************************************************

    // Dans BowlingBall.cpp (ou équivalent)
// #include <btBulletDynamicsCommon.h>

void BowlingBall::launch(const Ogre::Vector3& direction, float power, float spinAngularVelocityY) {
    // ... (calcul de la vélocité linéaire basé sur direction et power)
    // mRigidBody->setLinearVelocity(btVector3(linearVelocity.x, linearVelocity.y, linearVelocity.z));

    // Appliquer le spin comme une vélocité angulaire
    // Si 'direction' est principalement le long de l'axe Z mondial,
    // et que la boule est orientée de manière standard (son axe Y local est aligné avec l'axe Y mondial),
    // alors une vélocité angulaire autour de l'axe Y local (ou mondial) produira un spin latéral.
    if (mRigidBody) { // Assurez-vous que le corps rigide existe
        // La direction du spin est perpendiculaire à la direction du mouvement et à l'axe vertical.
        // Si la direction de lancer est (Dx, Dy, Dz), et l'axe vertical est (0,1,0),
        // l'axe de spin pour un effet latéral pourrait être le produit vectoriel de la direction de lancer
        // et de l'axe vertical, ou plus simplement, si le lancer est principalement en Z,
        // une rotation autour de Y est un bon point de départ.

        // Exemple simple : spin autour de l'axe Y local de la boule.
        // Convertir la vélocité angulaire de spin (qui est une magnitude) en un vecteur de vélocité angulaire.
        // L'axe de rotation pour le spin est typiquement l'axe vertical (Y) de la boule.
        btVector3 angularVelocity(0, spinAngularVelocityY, 0);

        // Si vous voulez que le spin soit relatif à la direction du lancer (plus complexe) :
        // Ogre::Vector3 forwardVector = direction.normalisedCopy();
        // Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Y;
        // Ogre::Vector3 spinAxis = forwardVector.crossProduct(upVector); // Axe de rotation perpendiculaire au mouvement et à la verticale
        // btVector3 angularVelocity = btVector3(spinAxis.x, spinAxis.y, spinAxis.z) * spinMagnitude; // spinMagnitude serait votre 'spin'

        mRigidBody->setAngularVelocity(angularVelocity);

        // Activer la boule pour qu'elle ne soit pas désactivée par le moteur physique
        mRigidBody->activate(true);
    }

    isNowRolling = true; // Mettre à jour l'état de la boule
    // ... (autre logique de lancement)
}
