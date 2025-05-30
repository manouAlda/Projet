cmake_minimum_required(VERSION 3.14)
project(MonBowling)

# Trouver les paquets nécéssaire
find_package(OGRE REQUIRED)
find_package(Bullet REQUIRED)
#find_package(OIS REQUIRED)

# Configuration FMOD
set(FMOD_ROOT ${CMAKE_SOURCE_DIR}/media/sounds/api/core)
set(FMOD_INCLUDE_DIR ${FMOD_ROOT}/inc)

# Détecter l'architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(FMOD_ARCH "x86_64")
else()
    set(FMOD_ARCH "x86")
endif()

# Chemins des bibliothèques selon l'OS
set(FMOD_LIBRARY ${FMOD_ROOT}/lib/${FMOD_ARCH}/libfmod.so)
set(FMOD_LIBRARY_DEBUG ${FMOD_ROOT}/lib/${FMOD_ARCH}/libfmodL.so)

# Inclure les répertoires d'en-tête
include_directories(${OGRE_INCLUDE_DIRS} ${FMOD_INCLUDE_DIR} ${BULLET_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

# Ajouter les fichiers source
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Créer l'exécutable
add_executable(BowlingGame ${SOURCES})

# Lier les bibliothèques
target_link_libraries(BowlingGame ${OGRE_LIBRARIES} ${BULLET_LIBRARIES} ${OIS_LIBRARIES} optimized ${FMOD_LIBRARY} debug ${FMOD_LIBRARY_DEBUG})

# Copier les fichiers de configuration
configure_file(${CMAKE_SOURCE_DIR}/resources.cfg ${CMAKE_BINARY_DIR}/resources.cfg COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/plugins.cfg ${CMAKE_BINARY_DIR}/plugins.cfg COPYONLY)

# Définir le RPATH pour trouver les .so. Copier les DLL/SO au moment du build 
set_target_properties(BowlingGame PROPERTIES INSTALL_RPATH "${FMOD_ROOT}/lib/${FMOD_ARCH}"
        BUILD_WITH_INSTALL_RPATH TRUE)

# Afficher ou est le ${CMAKE_SOURCE_DIR}
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
# Afficher ou est le ${CMAKE_BINARY_DIR}
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")