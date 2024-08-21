#include "Game.h"

int main() {
    InitWindow(1200, 600, "Lunar Lander");
    SetTargetFPS(60);

    InitGame();

    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    CloseWindow();
    return 0;
}
