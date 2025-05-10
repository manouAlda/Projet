cmake_minimum_required(VERSION 3.0)
project(Robber)

find_package(OGRE REQUIRED)
find_package(Bullet REQUIRED)

# Inclure les dossiers
include_directories(
    ${OGRE_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Header
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(SOURCES
    src/main.cpp
    src/Source/World.cpp
    src/Source/Physics.cpp
    src/Source/MouseHandler.cpp
    src/Source/PhysicsFrameListener.cpp
    src/Source/Plane.cpp
    src/Source/Player.cpp
    src/Source/Bullet/DebugDrawer.cpp
    src/Source/Bullet/DynamicsWorld.cpp
)

set(HEADER
    src/Header/World.hpp
    src/Header/Physics.hpp
    src/Header/MouseHandler.hpp
    src/Header/PhysicsFrameListener.hpp
    src/Header/Plane.hpp
    src/Header/Player.hpp
    src/Header/Bullet/DebugDrawer.hpp
    src/Header/Bullet/DynamicsWorld.hpp
)

# INCLUDE_DIRECTORIES(/home/mit/ogre-14.3.4/OgreMain/include/)
# INCLUDE_DIRECTORIES(/home/mit/ogre-14.3.4/build/include/)
# INCLUDE_DIRECTORIES(/home/mit/ogre-14.3.4/build/include/bullet/)

INCLUDE_DIRECTORIES(/home/linda/L3/Dev/ogre-14.3.4/OgreMain/include/)
INCLUDE_DIRECTORIES(/home/linda/L3/Dev/ogre-14.3.4/build/include/)
INCLUDE_DIRECTORIES(/home/linda/L3/Dev/ogre-14.3.4/build/include/bullet/)


INCLUDE_DIRECTORIES(/usr/local/include/OGRE)
include_directories(${OGRE_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS})

add_executable(main ${SOURCES} ${HEADER})

target_link_libraries(main ${OGRE_LIBRARIES} ${BULLET_LIBRARIES} OgreMain OgreBites OgreTerrain OgreBullet)