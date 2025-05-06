#include "SoundManager.h"
#include "Level.h"
#include "Guard.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

// Structure d'un fichier WAV pour le chargement
#pragma pack(push, 1)
struct WavHeader {
    char riff[4];              // "RIFF"
    unsigned int fileSize;      // Taille du fichier - 8
    char wave[4];              // "WAVE"
    char fmt[4];               // "fmt "
    unsigned int fmtSize;       // Taille du bloc fmt (16)
    unsigned short audioFormat; // Format audio (1 = PCM)
    unsigned short channels;    // Nombre de canaux
    unsigned int sampleRate;    // Taux d'échantillonnage
    unsigned int byteRate;      // Débit en octets par seconde
    unsigned short blockAlign;  // Alignement des blocs
    unsigned short bitsPerSample; // Bits par échantillon
    char data[4];              // "data"
    unsigned int dataSize;      // Taille des données audio
};
#pragma pack(pop)

SoundManager::SoundManager() 
    : mMasterVolume(1.0f)
    , mDefaultSoundRadius(15.0f)
    , mNextEmitterId(1)
    , mOcclusionFactor(0.4f)
    , mDistanceAttenuation(0.1f)
#ifdef USE_OPENAL
    , mDevice(nullptr)
    , mContext(nullptr)
#endif
#ifdef USE_FMOD
    , mSystem(nullptr)
#endif
{
}

SoundManager::~SoundManager() {
    shutdown();
}

bool SoundManager::initialize() {
#ifdef USE_OPENAL
    // Initialisation d'OpenAL
    mDevice = alcOpenDevice(nullptr); // Utiliser l'appareil par défaut
    if (!mDevice) {
        std::cerr << "Failed to open default OpenAL device" << std::endl;
        return false;
    }
    
    mContext = alcCreateContext(mDevice, nullptr);
    if (!mContext) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        alcCloseDevice(mDevice);
        mDevice = nullptr;
        return false;
    }
    
    alcMakeContextCurrent(mContext);
    
    // Vérifier les erreurs
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "Error during OpenAL initialization" << std::endl;
        shutdown();
        return false;
    }
    
    // Définir les propriétés de l'écouteur
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    float orientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f }; // forward and up vectors
    alListenerfv(AL_ORIENTATION, orientation);
#endif

#ifdef USE_FMOD
    // Initialisation de FMOD
    FMOD_RESULT result;
    result = FMOD::System_Create(&mSystem);
    if (result != FMOD_OK) {
        std::cerr << "Failed to create FMOD system: " << FMOD_ErrorString(result) << std::endl;
        return false;
    }
    
    result = mSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cerr << "Failed to initialize FMOD system: " << FMOD_ErrorString(result) << std::endl;
        mSystem->release();
        mSystem = nullptr;
        return false;
    }
#endif

    return true;
}

void SoundManager::shutdown() {
#ifdef USE_OPENAL
    // Arrêter et supprimer toutes les sources
    for (auto& emitter : mSoundEmitters) {
        alDeleteSources(1, &emitter.second.source);
    }
    mSoundEmitters.clear();
    
    // Supprimer tous les buffers
    for (auto& sound : mSoundLibrary) {
        alDeleteBuffers(1, &sound.second.buffer);
    }
    mSoundLibrary.clear();
    
    // Fermer OpenAL
    if (mContext) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(mContext);
        mContext = nullptr;
    }
    
    if (mDevice) {
        alcCloseDevice(mDevice);
        mDevice = nullptr;
    }
#endif

#ifdef USE_FMOD
    // Arrêter et libérer tous les sons
    for (auto& emitter : mSoundEmitters) {
        if (emitter.second.channel) {
            emitter.second.channel->stop();
        }
    }
    mSoundEmitters.clear();
    
    // Libérer tous les sons
    for (auto& sound : mSoundLibrary) {
        if (sound.second.sound) {
            sound.second.sound->release();
        }
    }
    mSoundLibrary.clear();
    
    // Fermer FMOD
    if (mSystem) {
        mSystem->close();
        mSystem->release();
        mSystem = nullptr;
    }
#endif
}

bool SoundManager::loadSound(const std::string& name, const std::string& filename) {
    // Vérifier si le son existe déjà
    if (mSoundLibrary.find(name) != mSoundLibrary.end()) {
        return true; // Son déjà chargé
    }
    
    Sound newSound;
    newSound.name = name;
    
    // Déterminer le type de fichier par l'extension
    std::string extension;
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filename.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }
    
    bool loaded = false;
    if (extension == "wav") {
        loaded = loadWavFile(filename, newSound);
    } else if (extension == "ogg") {
        loaded = loadOggFile(filename, newSound);
    } else {
        std::cerr << "Unsupported audio format: " << extension << std::endl;
        return false;
    }
    
    if (loaded) {
        mSoundLibrary[name] = std::move(newSound);
        return true;
    }
    
    return false;
}

bool SoundManager::loadWavFile(const std::string& filename, Sound& sound) {
#ifdef USE_OPENAL
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        return false;
    }
    
    WavHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    
    if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE" ||
        std::string(header.fmt, 4) != "fmt " || std::string(header.data, 4) != "data") {
        std::cerr << "Invalid WAV file format: " << filename << std::endl;
        file.close();
        return false;
    }
    
    if (header.audioFormat != 1) { // PCM format
        std::cerr << "Unsupported WAV format (non-PCM): " << filename << std::endl;
        file.close();
        return false;
    }
    
    // Allocate memory for audio data
    char* data = new char[header.dataSize];
    file.read(data, header.dataSize);
    file.close();
    
    // Create OpenAL buffer
    alGenBuffers(1, &sound.buffer);
    
    // Determine format based on channels and bits per sample
    ALenum format;
    if (header.channels == 1) {
        format = (header.bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else if (header.channels == 2) {
        format = (header.bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    } else {
        std::cerr << "Unsupported WAV channels: " << header.channels << std::endl;
        delete[] data;
        alDeleteBuffers(1, &sound.buffer);
        return false;
    }
    
    // Load data into the buffer
    alBufferData(sound.buffer, format, data, header.dataSize, header.sampleRate);
    
    // Clean up
    delete[] data;
    
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "OpenAL error loading WAV file: " << filename << std::endl;
        alDeleteBuffers(1, &sound.buffer);
        return false;
    }
    
    return true;
#endif

#ifdef USE_FMOD
    FMOD_RESULT result;
    result = mSystem->createSound(filename.c_str(), FMOD_DEFAULT, nullptr, &sound.sound);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error loading WAV file: " << filename << " - " 
                  << FMOD_ErrorString(result) << std::endl;
        return false;
    }
    return true;
#endif

    return false; // Si aucune bibliothèque n'est définie
}

bool SoundManager::loadOggFile(const std::string& filename, Sound& sound) {
    // Le chargement de fichiers OGG est similaire à WAV avec OpenAL mais nécessite
    // une bibliothèque comme libogg/libvorbis ou stb_vorbis

#ifdef USE_OPENAL
    // Version simplifiée, en pratique vous devriez utiliser libogg et libvorbis
    std::cerr << "OGG loading not implemented for OpenAL. Use external library." << std::endl;
    return false;
#endif

#ifdef USE_FMOD
    // FMOD gère nativement les fichiers OGG
    FMOD_RESULT result;
    result = mSystem->createSound(filename.c_str(), FMOD_DEFAULT, nullptr, &sound.sound);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error loading OGG file: " << filename << " - " 
                  << FMOD_ErrorString(result) << std::endl;
        return false;
    }
    return true;
#endif

    return false;
}

int SoundManager::playSoundAtPosition(const std::string& name, const Ogre::Vector3& position, 
                                    float volume, bool loop, float priority) {
    // Vérifier si le son existe
    auto it = mSoundLibrary.find(name);
    if (it == mSoundLibrary.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return -1;
    }
    
    // Créer une nouvelle instance d'émetteur
    SoundEmitter emitter;
    emitter.position = position;
    emitter.radius = mDefaultSoundRadius;
    emitter.volume = volume;
    emitter.priority = priority;
    emitter.isPlaying = true;
    emitter.isLooping = loop;
    emitter.soundName = name;
    
    int emitterId = mNextEmitterId++;
    
#ifdef USE_OPENAL
    // Créer une source OpenAL
    alGenSources(1, &emitter.source);
    
    // Configurer la source
    alSourcei(emitter.source, AL_BUFFER, it->second.buffer);
    alSource3f(emitter.source, AL_POSITION, position.x, position.y, position.z);
    alSource3f(emitter.source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcef(emitter.source, AL_GAIN, volume * mMasterVolume);
    alSourcef(emitter.source, AL_REFERENCE_DISTANCE, 1.0f);
    alSourcef(emitter.source, AL_MAX_DISTANCE, emitter.radius);
    alSourcei(emitter.source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    
    // Jouer le son
    alSourcePlay(emitter.source);
    
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "OpenAL error playing sound: " << name << std::endl;
        alDeleteSources(1, &emitter.source);
        return -1;
    }
#endif

#ifdef USE_FMOD
    // Créer un canal FMOD
    FMOD_RESULT result;
    FMOD::Channel* channel = nullptr;
    
    FMOD_MODE mode = FMOD_3D | FMOD_3D_LINEARROLLOFF;
    if (loop) {
        mode |= FMOD_LOOP_NORMAL;
    }
    
    // Définir le mode du son si ce n'est pas déjà fait
    it->second.sound->setMode(mode);
    
    // Jouer le son
    result = mSystem->playSound(it->second.sound, nullptr, false, &channel);
    if (result != FMOD_OK) {
        std::cerr << "FMOD error playing sound: " << name << " - " 
                  << FMOD_ErrorString(result) << std::endl;
        return -1;
    }
    
    // Configurer le canal
    FMOD_VECTOR fmodPos = { position.x, position.y, position.z };
    FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
    
    channel->set3DAttributes(&fmodPos, &vel);
    channel->setVolume(volume * mMasterVolume);
    channel->set3DMinMaxDistance(1.0f, emitter.radius);
    
    emitter.channel = channel;
#endif

    // Ajouter l'émetteur à la liste
    mSoundEmitters[emitterId] = emitter;
    
    // Notifier les gardes du son (pour l'IA)
    notifyGuardsOfSound(position, volume);
    
    return emitterId;
}

void SoundManager::stopSound(int emitterId) {
    auto it = mSoundEmitters.find(emitterId);
    if (it == mSoundEmitters.end()) {
        return;
    }
    
#ifdef USE_OPENAL
    alSourceStop(it->second.source);
    alDeleteSources(1, &it->second.source);
#endif

#ifdef USE_FMOD
    if (it->second.channel) {
        it->second.channel->stop();
    }
#endif

    mSoundEmitters.erase(it);
}

void SoundManager::setSoundVolume(int emitterId, float volume) {
    auto it = mSoundEmitters.find(emitterId);
    if (it == mSoundEmitters.end()) {
        return;
    }
    
    it->second.volume = volume;
    
#ifdef USE_OPENAL
    alSourcef(it->second.source, AL_GAIN, volume * mMasterVolume);
#endif

#ifdef USE_FMOD
    if (it->second.channel) {
        it->second.channel->setVolume(volume * mMasterVolume);
    }
#endif
}

void SoundManager::setSoundPosition(int emitterId, const Ogre::Vector3& position) {
    auto it = mSoundEmitters.find(emitterId);
    if (it == mSoundEmitters.end()) {
        return;
    }
    
    it->second.position = position;
    
#ifdef USE_OPENAL
    alSource3f(it->second.source, AL_POSITION, position.x, position.y, position.z);
#endif

#ifdef USE_FMOD
    if (it->second.channel) {
        FMOD_VECTOR fmodPos = { position.x, position.y, position.z };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
        it->second.channel->set3DAttributes(&fmodPos, &vel);
    }
#endif
}

void SoundManager::setMasterVolume(float volume) {
    mMasterVolume = std::max(0.0f, std::min(1.0f, volume));
    
    // Mettre à jour tous les émetteurs
    for (auto& emitter : mSoundEmitters) {
#ifdef USE_OPENAL
        alSourcef(emitter.second.source, AL_GAIN, emitter.second.volume * mMasterVolume);
#endif

#ifdef USE_FMOD
        if (emitter.second.channel) {
            emitter.second.channel->setVolume(emitter.second.volume * mMasterVolume);
        }
#endif
    }
}

void SoundManager::update(const Ogre::Vector3& listenerPosition, const Ogre::Vector3& listenerDirection, 
                        const Ogre::Vector3& listenerUp, Level* level) {
    // Mettre à jour la position de l'écouteur
#ifdef USE_OPENAL
    alListener3f(AL_POSITION, listenerPosition.x, listenerPosition.y, listenerPosition.z);
    
    // Orientation (au format At-Up)
    float orientation[6];
    orientation[0] = listenerDirection.x;
    orientation[1] = listenerDirection.y;
    orientation[2] = listenerDirection.z;
    orientation[3] = listenerUp.x;
    orientation[4] = listenerUp.y;
    orientation[5] = listenerUp.z;
    
    alListenerfv(AL_ORIENTATION, orientation);
#endif

#ifdef USE_FMOD
    FMOD_VECTOR fmodPos = { listenerPosition.x, listenerPosition.y, listenerPosition.z };
    FMOD_VECTOR fmodVel = { 0.0f, 0.0f, 0.0f }; // Pas de vélocité pour simplifier
    FMOD_VECTOR fmodForward = { listenerDirection.x, listenerDirection.y, listenerDirection.z };
    FMOD_VECTOR fmodUp = { listenerUp.x, listenerUp.y, listenerUp.z };
    
    mSystem->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
    mSystem->update();
#endif

    // Mettre à jour tous les émetteurs (occlusion, etc.)
    for (auto it = mSoundEmitters.begin(); it != mSoundEmitters.end(); ) {
        SoundEmitter& emitter = it->second;
        
        // Vérifier si le son est toujours en cours de lecture
        bool isPlaying = false;
        
#ifdef USE_OPENAL
        ALint state;
        alGetSourcei(emitter.source, AL_SOURCE_STATE, &state);
        isPlaying = (state == AL_PLAYING);
#endif

#ifdef USE_FMOD
        if (emitter.channel) {
            bool playing = false;
            emitter.channel->isPlaying(&playing);
            isPlaying = playing;
        }
#endif

        if (!isPlaying && !emitter.isLooping) {
            // Supprimer les sons qui ne sont plus en cours de lecture
#ifdef USE_OPENAL
            alDeleteSources(1, &emitter.source);
#endif
            it = mSoundEmitters.erase(it);
            continue;
        }
        
        // Calculer l'occlusion si un niveau est fourni
        if (level) {
            float occlusionFactor = calculateOcclusion(emitter.position, listenerPosition, level);
            
            // Appliquer l'occlusion au volume
            float actualVolume = emitter.volume * (1.0f - occlusionFactor) * mMasterVolume;
            
#ifdef USE_OPENAL
            alSourcef(emitter.source, AL_GAIN, actualVolume);
#endif

#ifdef USE_FMOD
            if (emitter.channel) {
                emitter.channel->setVolume(actualVolume);
            }
#endif
        }
        
        ++it;
    }
}

float SoundManager::calculateOcclusion(const Ogre::Vector3& sourcePos, const Ogre::Vector3& listenerPos, Level* level) {
    // Calculer l'occlusion avec des rayons multiples pour plus de précision
    return multipleRayOcclusion(sourcePos, listenerPos, level);
}

float SoundManager::multipleRayOcclusion(const Ogre::Vector3& sourcePos, const Ogre::Vector3& listenerPos, 
                                      Level* level, int rayCount) {
    // Distance entre la source et l'écouteur
    float distance = (sourcePos - listenerPos).length();
    
    // Atténuation basée sur la distance
    float distanceAttenuation = 1.0f - std::min(1.0f, distance * mDistanceAttenuation / mDefaultSoundRadius);
    
    // Si les points sont très proches, pas d'occlusion
    if (distance < 0.5f) {
        return 0.0f;
    }
    
    // Occlusion par obstacles (rayons)
    float totalOcclusion = 0.0f;
    
    // Rayon principal direct
    Ogre::Vector3 direction = (listenerPos - sourcePos).normalisedCopy();
    Ogre::Ray ray(sourcePos, direction);
    
    std::pair<bool, float> rayResult = level->raycastToPoint(ray, distance);
    if (rayResult.first && rayResult.second < distance - 0.1f) {
        // Un obstacle est détecté sur le chemin direct
        totalOcclusion += 1.0f;
    }
    
    // Rayons supplémentaires légèrement décalés pour détecter les obstacles partiels
    if (rayCount > 1) {
        // Trouver un vecteur perpendiculaire pour créer des rayons décalés
        Ogre::Vector3 perpDirection;
        if (std::abs(direction.y) < 0.9f) {
            perpDirection = direction.crossProduct(Ogre::Vector3::UNIT_Y).normalisedCopy();
        } else {
            perpDirection = direction.crossProduct(Ogre::Vector3::UNIT_X).normalisedCopy();
        }
        
        Ogre::Vector3 upVector = direction.crossProduct(perpDirection).normalisedCopy();
        
        // Répartir les rayons supplémentaires
        for (int i = 1; i < rayCount; ++i) {
            float angle = 2.0f * M_PI * i / (rayCount - 1);
            float radius = 0.3f; // Petit rayon autour du rayon principal
            
            Ogre::Vector3 offset = perpDirection * std::cos(angle) * radius + 
                                  upVector * std::sin(angle) * radius;
            
            // Source et destination légèrement décalées
            Ogre::Vector3 offsetSourcePos = sourcePos + offset;
            Ogre::Vector3 offsetListenerPos = listenerPos + offset;
            
            direction = (offsetListenerPos - offsetSourcePos).normalisedCopy();
            ray = Ogre::Ray(offsetSourcePos, direction);
            
            distance = (offsetListenerPos - offsetSourcePos).length();
            rayResult = level->raycastToPoint(ray, distance);
            
            if (rayResult.first && rayResult.second < distance - 0.1f) {
                totalOcclusion += 1.0f;
            }
        }
        
        // Normaliser l'occlusion
        totalOcclusion /= rayCount;
    }
    
    // Combiner l'occlusion et l'atténuation par distance
    return totalOcclusion * mOcclusionFactor + (1.0f - distanceAttenuation);
}

void SoundManager::notifyGuardsOfSound(const Ogre::Vector3& position, float intensity) {
    // Notifier tous les callbacks enregistrés
    for (const auto& callback : mSoundCallbacks) {
        callback(position, intensity);
    }
}

void SoundManager::createFootstepSound(const Ogre::Vector3& position, float volume) {
    // Jouer un son de pas aléatoire
    static const std::array<std::string, 4> footstepSounds = {
        "footstep1", "footstep2", "footstep3", "footstep4"
    };
    
    // Sélectionner un son de pas aléatoirement
    int randomIndex = rand() % footstepSounds.size();
    std::string soundName = footstepSounds[randomIndex];
    
    // Vérifier si le son existe
    if (mSoundLibrary.find(soundName) == mSoundLibrary.end()) {
        // Si le son n'existe pas, utiliser un son par défaut
        soundName = "footstep1";
        
        // Si ce son n'existe pas non plus, ne rien faire
        if (mSoundLibrary.find(soundName) == mSoundLibrary.end()) {
            return;
        }
    }
    
    // Jouer le son avec un volume réduit et un rayon plus petit
    float footstepRadius = mDefaultSoundRadius * 0.5f;
    int emitterId = playSoundAtPosition(soundName, position, volume, false, 0.3f);
    
    // Modifier le rayon de ce son spécifique
    auto it = mSoundEmitters.find(emitterId);
    if (it != mSoundEmitters.end()) {
        it->second.radius = footstepRadius;
        
#ifdef USE_OPENAL
        alSourcef(it->second.source, AL_MAX_DISTANCE, footstepRadius);
#endif

#ifdef USE_FMOD
        if (it->second.channel) {
            it->second.channel->set3DMinMaxDistance(1.0f, footstepRadius);
        }
#endif
    }
    
    // Notifier les gardes, mais avec une intensité réduite
    notifyGuardsOfSound(position, volume * 0.5f);
}

void SoundManager::createAlertSound(const Ogre::Vector3& position, float volume) {
    // Jouer un son d'alerte
    std::string soundName = "alert";
    
    // Vérifier si le son existe
    if (mSoundLibrary.find(soundName) == mSoundLibrary.end()) {
        std::cerr << "Alert sound not found in library" << std::endl;
        return;
    }
    
    // Jouer le son avec un volume élevé et un rayon plus grand
    float alertRadius = mDefaultSoundRadius * 1.5f;
    int emitterId = playSoundAtPosition(soundName, position, volume, false, 0.8f);
    
    // Modifier le rayon de ce son spécifique
    auto it = mSoundEmitters.find(emitterId);
    if (it != mSoundEmitters.end()) {
        it->second.radius = alertRadius;
        
#ifdef USE_OPENAL
        alSourcef(it->second.source, AL_MAX_DISTANCE, alertRadius);
#endif

#ifdef USE_FMOD
        if (it->second.channel) {
            it->second.channel->set3DMinMaxDistance(1.0f, alertRadius);
        }
#endif
    }
    
    // Notifier les gardes avec une intensité élevée
    notifyGuardsOfSound(position, volume * 1.5f);
}

// Ajouter une fonction callback pour la notification des sons
void SoundManager::addSoundNotificationCallback(SoundNotificationCallback callback) {
    mSoundCallbacks.push_back(callback);
}

// Supprimer un callback de notification des sons
void SoundManager::removeSoundNotificationCallback(SoundNotificationCallback callback) {
    // En pratique, cette implémentation est simplifiée
    // car il est difficile de comparer des std::function directement
    // Une meilleure approche serait d'utiliser un identifiant
    
    // Simplement vider la liste pour l'exemple
    mSoundCallbacks.clear();
}

// Méthode pour vérifier si un son spécifique est joué actuellement
bool SoundManager::isSoundPlaying(int emitterId) const {
    auto it = mSoundEmitters.find(emitterId);
    if (it == mSoundEmitters.end()) {
        return false;
    }
    
    bool isPlaying = false;
    
#ifdef USE_OPENAL
    ALint state;
    alGetSourcei(it->second.source, AL_SOURCE_STATE, &state);
    isPlaying = (state == AL_PLAYING);
#endif

#ifdef USE_FMOD
    if (it->second.channel) {
        bool playing = false;
        it->second.channel->isPlaying(&playing);
        isPlaying = playing;
    }
#endif

    return isPlaying;
}

// Méthode pour ajuster le facteur d'occlusion
void SoundManager::setOcclusionFactor(float factor) {
    mOcclusionFactor = std::max(0.0f, std::min(1.0f, factor));
}

// Méthode pour ajuster le facteur d'atténuation par distance
void SoundManager::setDistanceAttenuation(float factor) {
    mDistanceAttenuation = std::max(0.0f, factor);
}

// Méthode pour obtenir la liste des sons actuellement joués
std::vector<int> SoundManager::getActiveSoundEmitters() const {
    std::vector<int> activeEmitters;
    
    for (const auto& pair : mSoundEmitters) {
        if (isSoundPlaying(pair.first)) {
            activeEmitters.push_back(pair.first);
        }
    }
    
    return activeEmitters;
}

// Méthode pour mettre en pause/reprendre un son
void SoundManager::pauseSound(int emitterId, bool pause) {
    auto it = mSoundEmitters.find(emitterId);
    if (it == mSoundEmitters.end()) {
        return;
    }
    
#ifdef USE_OPENAL
    if (pause) {
        alSourcePause(it->second.source);
    } else {
        alSourcePlay(it->second.source);
    }
#endif

#ifdef USE_FMOD
    if (it->second.channel) {
        it->second.channel->setPaused(pause);
    }
#endif
}

// Méthode pour mettre en pause/reprendre tous les sons
void SoundManager::pauseAllSounds(bool pause) {
    for (auto& pair : mSoundEmitters) {
#ifdef USE_OPENAL
        if (pause) {
            alSourcePause(pair.second.source);
        } else {
            alSourcePlay(pair.second.source);
        }
#endif

#ifdef USE_FMOD
        if (pair.second.channel) {
            pair.second.channel->setPaused(pause);
        }
#endif
    }
}

// Méthode pour arrêter tous les sons
void SoundManager::stopAllSounds() {
    for (auto& pair : mSoundEmitters) {
#ifdef USE_OPENAL
        alSourceStop(pair.second.source);
        alDeleteSources(1, &pair.second.source);
#endif

#ifdef USE_FMOD
        if (pair.second.channel) {
            pair.second.channel->stop();
        }
#endif
    }
    
    mSoundEmitters.clear();
}

// Méthode pour obtenir des informations sur un émetteur sonore
bool SoundManager::getSoundEmitterInfo(int emitterId, Ogre::Vector3& outPosition, float& outVolume, 
                                     std::string& outSoundName) const {
    auto it = mSoundEmitters.find(emitterId);
    if (it == mSoundEmitters.end()) {
        return false;
    }
    
    const SoundEmitter& emitter = it->second;
    outPosition = emitter.position;
    outVolume = emitter.volume;
    outSoundName = emitter.soundName;
    
    return true;
}

// Méthode utilitaire pour estimer l'intensité d'un son à une position donnée
float SoundManager::estimateSoundIntensityAt(int emitterId, const Ogre::Vector3& position, Level* level) const {
    auto it = mSoundEmitters.find(emitterId);
    if (it == mSoundEmitters.end()) {
        return 0.0f;
    }
    
    const SoundEmitter& emitter = it->second;
    
    // Calculer la distance
    float distance = (position - emitter.position).length();
    
    // Si hors de portée, pas d'intensité
    if (distance > emitter.radius) {
        return 0.0f;
    }
    
    // Atténuation basée sur la distance
    float attenuation = 1.0f - distance / emitter.radius;
    
    // Si un niveau est fourni, tenir compte de l'occlusion
    if (level) {
        float occlusion = calculateOcclusion(emitter.position, position, level);
        attenuation *= (1.0f - occlusion);
    }
    
    return emitter.volume * attenuation * mMasterVolume;
}