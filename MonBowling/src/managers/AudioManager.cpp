#include "../../include/managers/AudioManager.h" 

// Initialisation du pointeur statique
AudioManager* AudioManager::mInstance = nullptr;

// --- Singleton --- 

AudioManager* AudioManager::getInstance() {
    if (mInstance == nullptr) {
        mInstance = new AudioManager();
    }
    return mInstance;
}

AudioManager::AudioManager() : mFMODSystem(nullptr) {}

AudioManager::~AudioManager() {
    // Le shutdown devrait être appelé explicitement avant la destruction
    // mais par sécurité :
    shutdown();
}

// --- Initialisation / Fermeture --- 

bool AudioManager::initialize(const std::string& mediaPath) {
    mMediaPath = mediaPath;
    // Ajouter un / à la fin si nécessaire
    if (!mMediaPath.empty() && mMediaPath.back() != '/') {
        mMediaPath += '/';
    }

    FMOD_RESULT result;
    result = FMOD::System_Create(&mFMODSystem);
    if (!FMODErrorCheck(result)) {
        return false;
    }

    // Initialiser le système FMOD
    // 32 canaux virtuels max (ajuster si besoin)
    result = mFMODSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    if (!FMODErrorCheck(result)) {
        // Libérer le système en cas d'échec d'initialisation
        mFMODSystem->release();
        mFMODSystem = nullptr;
        return false;
    }

    Ogre::LogManager::getSingleton().logMessage("AudioManager: FMOD initialisé avec succès.");
    return true;
}

void AudioManager::shutdown() {
    if (mFMODSystem) {
        // Libérer tous les sons chargés
        for (auto const& [key, val] : mSounds) {
            if (val) {
                val->release();
            }
        }
        mSounds.clear();
        mChannels.clear(); // Les canaux sont invalidés quand le système est libéré

        // Fermer et libérer le système FMOD
        mFMODSystem->close();
        mFMODSystem->release();
        mFMODSystem = nullptr;
        Ogre::LogManager::getSingleton().logMessage("AudioManager: FMOD arrêté.");
    }
}

// --- Mise à jour --- 

void AudioManager::update() {
    if (mFMODSystem) {
        mFMODSystem->update();
    }
}

// --- Gestion des sons --- 

bool AudioManager::loadSound(const std::string& fileName, const std::string& soundName, bool loop) {
    if (!mFMODSystem) return false;

    // Vérifier si le son est déjà chargé
    if (mSounds.find(soundName) != mSounds.end()) {
        Ogre::LogManager::getSingleton().logMessage("AudioManager: Le son '" + soundName + "' est déjà chargé.");
        return true; // Déjà chargé
    }

    FMOD::Sound* sound = nullptr;
    std::string fullPath = mMediaPath + fileName;
    FMOD_MODE mode = FMOD_DEFAULT;
    if (loop) {
        mode |= FMOD_LOOP_NORMAL;
    } else {
        mode |= FMOD_LOOP_OFF;
    }

    FMOD_RESULT result = mFMODSystem->createSound(fullPath.c_str(), mode, nullptr, &sound);

    if (FMODErrorCheck(result)) {
        mSounds[soundName] = sound;
        Ogre::LogManager::getSingleton().logMessage("AudioManager: Son '" + soundName + "' chargé depuis '" + fullPath + "'.");
        return true;
    } else {
        Ogre::LogManager::getSingleton().logError("AudioManager: Echec du chargement du son '" + soundName + "' depuis '" + fullPath + "'.");
        return false;
    }
}

void AudioManager::playSound(const std::string& soundName) {
    if (!mFMODSystem) return;

    auto it = mSounds.find(soundName);
    if (it == mSounds.end() || it->second == nullptr) {
        Ogre::LogManager::getSingleton().logWarning("AudioManager: Tentative de lecture du son non chargé '" + soundName + "'.");
        return;
    }

    FMOD::Channel* channel = nullptr;
    // Jouer le son. Le canal peut être récupéré si on a besoin de le contrôler plus tard.
    FMOD_RESULT result = mFMODSystem->playSound(it->second, nullptr, false, &channel);

    if (FMODErrorCheck(result)) {
        // Stocker le canal si on a besoin de le contrôler (ex: arrêt d'une boucle)
        mChannels[soundName] = channel; // Écrase le canal précédent pour ce son
        // Ogre::LogManager::getSingleton().logMessage("AudioManager: Lecture du son '" + soundName + "'.");
    } else {
        Ogre::LogManager::getSingleton().logError("AudioManager: Echec de la lecture du son '" + soundName + "'.");
    }
}

void AudioManager::stopSound(const std::string& soundName) {
    auto it = mChannels.find(soundName);
    if (it != mChannels.end() && it->second != nullptr) {
        FMOD::Channel* channel = it->second;
        bool playing = false;
        channel->isPlaying(&playing);
        if (playing) {
            FMOD_RESULT result = channel->stop();
            FMODErrorCheck(result);
            // Ogre::LogManager::getSingleton().logMessage("AudioManager: Arrêt du son '" + soundName + "'.");
        }
        // Retirer le canal de la map une fois arrêté (ou laisser pour isPlaying?)
        // mChannels.erase(it);
    }
}

bool AudioManager::isPlaying(const std::string& soundName) {
    auto it = mChannels.find(soundName);
    if (it != mChannels.end() && it->second != nullptr) {
        FMOD::Channel* channel = it->second;
        bool playing = false;
        FMOD_RESULT result = channel->isPlaying(&playing);
        if (FMODErrorCheck(result)) {
            return playing;
        }
    }
    return false;
}

// --- Gestion des erreurs --- 

bool AudioManager::FMODErrorCheck(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        Ogre::LogManager::getSingleton().logError("AudioManager: Erreur FMOD! (" +
                                               Ogre::StringConverter::toString(result) + ") " +
                                               FMOD_ErrorString(result));
        return false;
    }
    return true;
}

