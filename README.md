# Lunar Lander

A simple Lunar Lander game built using C++ and the Raylib library. The game features basic physics simulation, terrain generation, particle effects, and a landing system.

## Features

- **Lunar Lander Simulation**: Control a lander with realistic physics, including gravity, thrust, and rotation.
- **Terrain Generation**: Procedurally generated terrain with multiple landing pads.
- **Assist Mode**: Optional assist mode to help stabilize and guide the lander to a safe landing.
- **Particle Effects**: Exhaust particles for a more realistic appearance during thrust.

## Project Structure

The project is organized as follows:

```
LunarLander/
├── Include/
│   ├── Game.h
│   ├── ParticleSystem.h
├── Src/
│   ├── main.cpp
│   ├── Game.cpp
│   ├── ParticleSystem.cpp
├── CMakeLists.txt
└── README.md
```

- **Include/**: Contains all the header files.
- **Src/**: Contains the implementation files (source code).
- **CMakeLists.txt**: The CMake configuration file for building the project.
- **README.md**: This file, providing an overview and instructions.

## Requirements

- **C++ Compiler**: Supports at least C++17.
- **CMake**: Version 3.10 or higher.
- **Raylib**: A simple and easy-to-use library to enjoy videogames programming.

## Installation

### 1. Install Dependencies

- **Raylib**: Make sure Raylib is installed on your system. You can install it via a package manager or from source. Examples:
  - **macOS**: `brew install raylib`
  - **Ubuntu**: `sudo apt-get install libraylib-dev`
  - **Windows**: Install via vcpkg or from [Raylib's website](https://www.raylib.com/).

### 2. Clone the Repository

```bash
git clone https://github.com/yourusername/lunar-lander.git
cd lunar-lander
```

### 3. Build the Project

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 4. Run the Game

After building, run the executable:

```bash
./LunarLander  # or LunarLander.exe on Windows
```

## Gameplay

- **Controls**:
  - `Arrow Up`: Apply thrust to the lander.
  - `Arrow Left/Right`: Rotate the lander.
  - `A`: Toggle assist mode.
  - `Enter`: Start the game or restart after a crash.

- **Objective**: Land the lunar lander safely on one of the landing pads. Avoid crashing into the terrain!

## Contributing

If you find any issues or have suggestions for improvements, feel free to submit a pull request or open an issue on the repository.

```

### Key Points:
- **Overview**: Introduces the project and its features.
- **Project Structure**: Provides a high-level view of the file organization.
- **Requirements & Installation**: Describes the dependencies and the steps to set up the project.
- **Gameplay**: Briefly describes the controls and objective.
- **Contributing**: Encourages community involvement.


