# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/linda/L3/git/projet/MonBowling

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/linda/L3/git/projet/MonBowling/build

# Include any dependencies generated for this target.
include CMakeFiles/BowlingGame.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/BowlingGame.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/BowlingGame.dir/flags.make

CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.o: ../src/core/AimingSystem.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/core/AimingSystem.cpp

CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/core/AimingSystem.cpp > CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.i

CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/core/AimingSystem.cpp -o CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.s

CMakeFiles/BowlingGame.dir/src/core/Application.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/core/Application.cpp.o: ../src/core/Application.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/BowlingGame.dir/src/core/Application.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/core/Application.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/core/Application.cpp

CMakeFiles/BowlingGame.dir/src/core/Application.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/core/Application.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/core/Application.cpp > CMakeFiles/BowlingGame.dir/src/core/Application.cpp.i

CMakeFiles/BowlingGame.dir/src/core/Application.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/core/Application.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/core/Application.cpp -o CMakeFiles/BowlingGame.dir/src/core/Application.cpp.s

CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.o: ../src/core/GameManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/core/GameManager.cpp

CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/core/GameManager.cpp > CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.i

CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/core/GameManager.cpp -o CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.s

CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.o: ../src/core/GameState.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/core/GameState.cpp

CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/core/GameState.cpp > CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.i

CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/core/GameState.cpp -o CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.s

CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.o: ../src/core/GameStateMachine.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/core/GameStateMachine.cpp

CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/core/GameStateMachine.cpp > CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.i

CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/core/GameStateMachine.cpp -o CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.s

CMakeFiles/BowlingGame.dir/src/main.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/BowlingGame.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/main.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/main.cpp

CMakeFiles/BowlingGame.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/main.cpp > CMakeFiles/BowlingGame.dir/src/main.cpp.i

CMakeFiles/BowlingGame.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/main.cpp -o CMakeFiles/BowlingGame.dir/src/main.cpp.s

CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.o: ../src/managers/AudioManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/managers/AudioManager.cpp

CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/managers/AudioManager.cpp > CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.i

CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/managers/AudioManager.cpp -o CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.s

CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.o: ../src/managers/CameraController.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/managers/CameraController.cpp

CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/managers/CameraController.cpp > CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.i

CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/managers/CameraController.cpp -o CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.s

CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.o: ../src/managers/CameraFollower.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/managers/CameraFollower.cpp

CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/managers/CameraFollower.cpp > CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.i

CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/managers/CameraFollower.cpp -o CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.s

CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.o: ../src/managers/PhysicsManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/managers/PhysicsManager.cpp

CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/managers/PhysicsManager.cpp > CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.i

CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/managers/PhysicsManager.cpp -o CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.s

CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.o: ../src/managers/ResourceManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/managers/ResourceManager.cpp

CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/managers/ResourceManager.cpp > CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.i

CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/managers/ResourceManager.cpp -o CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.s

CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.o: ../src/objects/BowlingBall.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/objects/BowlingBall.cpp

CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/objects/BowlingBall.cpp > CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.i

CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/objects/BowlingBall.cpp -o CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.s

CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.o: ../src/objects/BowlingLane.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/objects/BowlingLane.cpp

CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/objects/BowlingLane.cpp > CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.i

CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/objects/BowlingLane.cpp -o CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.s

CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.o: ../src/objects/BowlingObject.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/objects/BowlingObject.cpp

CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/objects/BowlingObject.cpp > CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.i

CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/objects/BowlingObject.cpp -o CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.s

CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.o: ../src/objects/BowlingPin.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/objects/BowlingPin.cpp

CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/objects/BowlingPin.cpp > CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.i

CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/objects/BowlingPin.cpp -o CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.s

CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.o: ../src/objects/ObjectFactory.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/objects/ObjectFactory.cpp

CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/objects/ObjectFactory.cpp > CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.i

CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/objects/ObjectFactory.cpp -o CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.s

CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.o: ../src/states/ScoreManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/states/ScoreManager.cpp

CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/states/ScoreManager.cpp > CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.i

CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/states/ScoreManager.cpp -o CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.s

CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.o: CMakeFiles/BowlingGame.dir/flags.make
CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.o: ../src/utils/PinDetector.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building CXX object CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.o -c /home/linda/L3/git/projet/MonBowling/src/utils/PinDetector.cpp

CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/linda/L3/git/projet/MonBowling/src/utils/PinDetector.cpp > CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.i

CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/linda/L3/git/projet/MonBowling/src/utils/PinDetector.cpp -o CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.s

# Object files for target BowlingGame
BowlingGame_OBJECTS = \
"CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/core/Application.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/main.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.o" \
"CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.o"

# External object files for target BowlingGame
BowlingGame_EXTERNAL_OBJECTS =

BowlingGame: CMakeFiles/BowlingGame.dir/src/core/AimingSystem.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/core/Application.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/core/GameManager.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/core/GameState.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/core/GameStateMachine.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/main.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/managers/AudioManager.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/managers/CameraController.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/managers/CameraFollower.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/managers/PhysicsManager.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/managers/ResourceManager.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/objects/BowlingBall.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/objects/BowlingLane.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/objects/BowlingObject.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/objects/BowlingPin.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/objects/ObjectFactory.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/states/ScoreManager.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/src/utils/PinDetector.cpp.o
BowlingGame: CMakeFiles/BowlingGame.dir/build.make
BowlingGame: /usr/local/lib/libOgreBites.so.14.3
BowlingGame: /usr/local/lib/libOgreBullet.so
BowlingGame: /usr/local/lib/libOgreMeshLodGenerator.so.14.3
BowlingGame: /usr/local/lib/libOgreOverlay.so.14.3
BowlingGame: /usr/local/lib/libOgreProperty.so.14.3
BowlingGame: /usr/local/lib/libOgreTerrain.so.14.3
BowlingGame: /usr/local/lib/libOgreVolume.so.14.3
BowlingGame: /usr/local/lib/libBulletDynamics.a
BowlingGame: /usr/local/lib/libBulletCollision.a
BowlingGame: /usr/local/lib/libLinearMath.a
BowlingGame: /usr/local/lib/libBulletSoftBody.a
BowlingGame: ../media/sounds/api/core/lib/x86_64/libfmod.so
BowlingGame: /usr/local/lib/libOgrePaging.so.14.3
BowlingGame: /usr/local/lib/libOgreRTShaderSystem.so.14.3
BowlingGame: /usr/local/lib/libOgreMain.so.14.3
BowlingGame: CMakeFiles/BowlingGame.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/linda/L3/git/projet/MonBowling/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Linking CXX executable BowlingGame"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BowlingGame.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/BowlingGame.dir/build: BowlingGame

.PHONY : CMakeFiles/BowlingGame.dir/build

CMakeFiles/BowlingGame.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/BowlingGame.dir/cmake_clean.cmake
.PHONY : CMakeFiles/BowlingGame.dir/clean

CMakeFiles/BowlingGame.dir/depend:
	cd /home/linda/L3/git/projet/MonBowling/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/linda/L3/git/projet/MonBowling /home/linda/L3/git/projet/MonBowling /home/linda/L3/git/projet/MonBowling/build /home/linda/L3/git/projet/MonBowling/build /home/linda/L3/git/projet/MonBowling/build/CMakeFiles/BowlingGame.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/BowlingGame.dir/depend

