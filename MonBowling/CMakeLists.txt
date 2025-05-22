cmake_minimum_required(VERSION 3.0)
project(MonBowling)

# Trouver les paquets nécéssaire
find_package(OGRE REQUIRED)
find_package(Bullet REQUIRED)
#find_package(OIS REQUIRED)

# Inclure les répertoires d'en-tête
include_directories(${OGRE_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

# Ajouter les fichiers source
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Créer l'exécutable
add_executable(BowlingGame ${SOURCES})

# Lier les bibliothèques
target_link_libraries(BowlingGame ${OGRE_LIBRARIES} ${BULLET_LIBRARIES} ${OIS_LIBRARIES})

# Copier les fichiers de configuration
configure_file(${CMAKE_SOURCE_DIR}/resources.cfg ${CMAKE_BINARY_DIR}/resources.cfg COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/plugins.cfg ${CMAKE_BINARY_DIR}/plugins.cfg COPYONLY)
