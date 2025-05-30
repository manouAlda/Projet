#include <fmod.h>
#include <fmod_errors.h>
#include <iostream>

int main() {
    FMOD_SYSTEM* system = nullptr;
    FMOD_RESULT result;
    
    // Ajouter le paramètre de version FMOD
    result = FMOD_System_Create(&system, FMOD_VERSION);
    if (result != FMOD_OK) {
        std::cerr << "Erreur FMOD: " << FMOD_ErrorString(result) << std::endl;
        return -1;
    }
    
    result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cerr << "Erreur init FMOD: " << FMOD_ErrorString(result) << std::endl;
        FMOD_System_Release(system);
        return -1;
    }
    
    std::cout << "FMOD initialisé avec succès !" << std::endl;
    
    FMOD_System_Close(system);
    FMOD_System_Release(system);
    return 0;
}