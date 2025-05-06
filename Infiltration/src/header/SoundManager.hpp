#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <Ogre.h>
#include <OgreBullet/Collisions/OgreBulletCollisionsRay.h>
#include <map>
#include <string>
#include <vector>
#include <memory>

// Vous pouvez utiliser OpenAL (plus simple à intégrer) ou FMOD (plus puissant)
// Décommentez la ligne appropriée selon votre choix
#define USE_OPENAL
//#define USE_FMOD

#ifdef USE_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#endif

#ifdef USE_FMOD
#include <fmod.hpp>
#include <fmod_errors.h>
#endif

class Level;

// Structure pour enregistrer les sources sonores dans l'environnement
struct SoundEmitter {
    Ogre::Vector3 position;
    float radius;
    float volume;
    float priority;
    bool isPlaying;
    bool isLooping;
    std::string soundName;
    
    #ifdef USE_OPENAL
    ALuint source;
    #endif
    
    #ifdef USE_FMOD
    FMOD::Channel* channel;
    #endif
};

// Structure pour un son chargé en mémoire
struct Sound {
    std::string name;
    
    #ifdef USE_OPENAL
    ALuint buffer;
    #endif
    
    #ifdef USE_FMOD
    FMOD::Sound* sound;
    #endif
};

// Classe pour gérer les sons et l'occlusion sonore
class SoundManager {
public:
    SoundManager();
    ~SoundManager();
    
    bool initialize();
    void shutdown();
    
    // Fonctions de base pour la gestion des sons
    bool loadSound(const std::string& name, const std::string& filename);
    int playSoundAtPosition(const std::string& name, const Ogre::Vector3& position, 
                          float volume = 1.0f, bool loop = false, float priority = 0.5f);
    void stopSound(int emitterId);
    void setSoundVolume(int emitterId, float volume);
    void setSoundPosition(int emitterId, const Ogre::Vector3& position);
    
    // Fonction principale pour mettre à jour les sons et l'occlusion
    void update(const Ogre::Vector3& listenerPosition, const Ogre::Vector3& listenerDirection, 
                const Ogre::Vector3& listenerUp, Level* level);
    
    // Définir le volume général
    void setMasterVolume(float volume);
    float getMasterVolume() const { return mMasterVolume; }
    
    // Fonctions pour créer des événements sonores
    void createFootstepSound(const Ogre::Vector3& position, float volume = 0.4f);
    void createAlertSound(const Ogre::Vector3& position, float volume = 1.0f);
    
    // Définir le rayon de propagation du son par défaut
    void setDefaultSoundRadius(float radius) { mDefaultSoundRadius = radius; }
    float getDefaultSoundRadius() const { return mDefaultSoundRadius; }
    
    // Fonction pour indiquer au guard.cpp qu'un son a été émis (utilisé pour l'IA)
    void notifyGuardsOfSound(const Ogre::Vector3& position, float intensity);
    
private:
    // Calculer le facteur d'occlusion entre deux points
    float calculateOcclusion(const Ogre::Vector3& sourcePos, const Ogre::Vector3& listenerPos, Level* level);
    
    // Lancer plusieurs rayons pour une détection d'occlusion plus précise
    float multipleRayOcclusion(const Ogre::Vector3& sourcePos, const Ogre::Vector3& listenerPos, Level* level, int rayCount = 5);
    
    // Callback pour les événements sonores (utilisé par les gardes)
    typedef std::function<void(const Ogre::Vector3&, float)> SoundNotificationCallback;
    std::vector<SoundNotificationCallback> mSoundCallbacks;
    
    // Données membres
    float mMasterVolume;
    float mDefaultSoundRadius;
    int mNextEmitterId;
    
    std::map<std::string, Sound> mSoundLibrary;
    std::map<int, SoundEmitter> mSoundEmitters;
    
    // Paramètres d'occlusion
    float mOcclusionFactor;      // Facteur d'atténuation par obstacle
    float mDistanceAttenuation;  // Facteur d'atténuation par distance
    
    #ifdef USE_OPENAL
    ALCdevice* mDevice;
    ALCcontext* mContext;
    #endif
    
    #ifdef USE_FMOD
    FMOD::System* mSystem;
    #endif
    
    // Fonctions utilitaires pour chargement de fichiers audio
    bool loadWavFile(const std::string& filename, Sound& sound);
    bool loadOggFile(const std::string& filename, Sound& sound);
};

#endif // SOUND_MANAGER_H