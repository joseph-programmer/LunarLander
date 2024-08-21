cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LunarLander)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(LunarLander
    Src/main.cpp
    Src/Game.cpp
    Src/ParticleSystem.cpp
)

# Include directories
target_include_directories(LunarLander PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Link Raylib (Make sure Raylib is installed on your system)
find_package(raylib REQUIRED)

# Link libraries to the target
target_link_libraries(LunarLander raylib)
