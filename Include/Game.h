#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <string>

// Constants
#define PLAYER_SIZE 20
#define GRAVITY 0.02f
#define THRUST 0.05f
#define ROTATION_SPEED 2.0f
#define FUEL_CONSUMPTION 0.5f
#define INITIAL_FUEL 100.0f
#define TERRAIN_POINTS 129 // Must be 2^n + 1
#define LANDING_PAD_WIDTH_MIN 30
#define LANDING_PAD_WIDTH 100
#define LANDING_PAD_HEIGHT 5
#define NUM_LANDING_PADS 7
#define ASSIST_DISTANCE 100.0f

// Structs
struct Player {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float fuel;
};

// Enums
enum GameState {
    SPLASH,
    GAME,
    GAME_OVER
};

enum CollisionReason {
    NONE,
    TERRAIN,
    LANDING_PAD_SUCCESS,
    LANDING_PAD_FAIL
};

void InitGame();
void UpdateGame();
void DrawGame();

#endif // GAME_H
