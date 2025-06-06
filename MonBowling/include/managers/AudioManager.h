// Fichier : AudioManager.h
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

// Inclure les en-têtes FMOD depuis le chemin fourni par l'utilisateur
// Il faudra s'assurer que le chemin d'inclusion est correctement configuré dans le projet/makefile
#include <fmod.hpp> // Chemin principal FMOD Core API
#include <fmod_errors.h> // Pour la gestion des erreurs FMOD

#include <string>
#include <map>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>

// Classe AudioManager (Singleton)
class AudioManager {
public:
    // Obtient l'instance unique (Singleton)
    static AudioManager* getInstance();

    // Initialisation et fermeture du système FMOD
    bool initialize(const std::string& mediaPath = "../media/sounds/son/"); // Chemin vers les fichiers audio
    void shutdown();

    // Mise à jour du système FMOD (à appeler à chaque frame)
    void update();

    // Chargement d'un son
    bool loadSound(const std::string& fileName, const std::string& soundName, bool loop = false);

    // Lecture d'un son
    void playSound(const std::string& soundName);

    // Arrêt d'un son (utile pour les sons en boucle)
    void stopSound(const std::string& soundName);

    // Vérifie si un son est en cours de lecture
    bool isPlaying(const std::string& soundName);

private:
    // Constructeur/Destructeur privés (Singleton)
    AudioManager();
    ~AudioManager();

    // Empêcher la copie
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // Vérification des erreurs FMOD
    bool FMODErrorCheck(FMOD_RESULT result);

    // Instance unique
    static AudioManager* mInstance;

    // Système FMOD
    FMOD::System* mFMODSystem;

    // Map pour stocker les sons chargés
    std::map<std::string, FMOD::Sound*> mSounds;

    // Map pour stocker les canaux actifs (pour les sons en boucle ou contrôlables)
    std::map<std::string, FMOD::Channel*> mChannels;

    // Chemin vers les médias
    std::string mMediaPath;
};

#endif // AUDIO_MANAGER_H

