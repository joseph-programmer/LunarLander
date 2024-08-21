#include "Game.h"
#include "ParticleSystem.h"

// Global Variables
Player player;

std::vector<Vector2> terrain;
std::vector<Rectangle> landingPads;
std::vector<Particle> exhaustParticles;

GameState currentState = SPLASH;
int score = 0;
float gameTime = 0;
bool assistMode = false;
CollisionReason collisionReason = NONE;

float Vector2Length(Vector2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float RandomNumber(double Lowest, double Highest, int DecimalPlaces, bool PreventZero) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(Lowest, Highest);

    float result;
    do {
        result = std::round(dis(gen) * std::pow(10, DecimalPlaces)) / std::pow(10, DecimalPlaces);
    } while (PreventZero && result == 0);

    return result;
}

void GenerateTerrain() {
    terrain.clear();
    landingPads.clear();

    float screenWidth = static_cast<float>(GetScreenWidth());
    float screenHeight = static_cast<float>(GetScreenHeight());

    float x = 0, y = screenHeight * 0.7f;
    float stepX = screenWidth / (TERRAIN_POINTS - 1);

    terrain.push_back({ 0, screenHeight }); // Left wall

    // Calculate the interval for pad placement
    int padInterval = (TERRAIN_POINTS - 1) / NUM_LANDING_PADS;
    int nextPadIndex = padInterval;
    int padsPlaced = 0;
    for (int i = 1; i < TERRAIN_POINTS; ++i) {
        x += stepX;
        y += RandomNumber(-20, 20, 0, false);
        y = std::clamp(y, screenHeight * 0.3f, screenHeight * 0.9f);

        // Place landing pad if at the correct interval
        if (i == nextPadIndex && padsPlaced < NUM_LANDING_PADS) {
            float padWidth = RandomNumber(LANDING_PAD_WIDTH_MIN, LANDING_PAD_WIDTH, 0, true);

            Rectangle pad = {
                x,
                y,
                padWidth,
                LANDING_PAD_HEIGHT
            };
            landingPads.push_back(pad);

            // Flatten terrain for landing pad
            terrain.push_back({ x, y });
            x += padWidth;
            terrain.push_back({ x, y });  // Ensure the terrain is flat across the pad

            // Update the nextPadIndex for the next pad
            nextPadIndex += padInterval;

            // Skip i forward to account for the width of the pad
            i += static_cast<int>(padWidth / stepX) - 1;
        }
        else {
            terrain.push_back({ x, y });
        }
    }

    terrain.push_back({ screenWidth, screenHeight }); // Right wall to complete the terrain
}

void InitGame() {
    player.position = { GetScreenWidth() / 2.0f, 50.0f };
    player.velocity = { 0, 0 };
    player.rotation = 0;
    player.fuel = INITIAL_FUEL;

    GenerateTerrain();

    score = 0;
    gameTime = 0;
    collisionReason = NONE;
}

bool CheckLanderCollision(Vector2 p1, Vector2 p2, Vector2 p3) {
    for (size_t i = 0; i < terrain.size() - 1; ++i) {
        if (CheckCollisionLines(p1, p2, terrain[i], terrain[i + 1], nullptr) ||
            CheckCollisionLines(p2, p3, terrain[i], terrain[i + 1], nullptr) ||
            CheckCollisionLines(p3, p1, terrain[i], terrain[i + 1], nullptr)) {
            return true;
        }
    }
    return false;
}

bool CheckSuccessfulLanding(Vector2 v1, Vector2 v2, Vector2 v3) {
    for (const auto& pad : landingPads) {
        if (CheckCollisionPointRec(v1, pad) || CheckCollisionPointRec(v2, pad) || CheckCollisionPointRec(v3, pad)) {
            if (std::abs(player.velocity.y) < 0.5f &&
                std::abs(player.velocity.x) < 0.5f &&
                std::abs(player.rotation) < 15.0f) {
                return true;
            }
        }
    }
    return false;
}

void ApplyAssist() {
    // Find the nearest landing pad
    float minDist = INFINITY;
    Rectangle nearestPad = { 0,0,0,0 };
    for (const auto& pad : landingPads) {
        float dist = Vector2Length({ player.position.x - (pad.x + pad.width / 2), player.position.y - pad.y });
        if (dist < minDist) {
            minDist = dist;
            nearestPad = pad;
        }
    }

    // If close to a landing pad, apply assist
    if (minDist < ASSIST_DISTANCE) {
        // Stabilize rotation
        player.rotation *= 0.95f;

        // Reduce horizontal velocity
        player.velocity.x *= 0.98f;

        // Reduce vertical velocity if too high
        if (player.velocity.y > 0.2f) {
            player.velocity.y *= 0.98f;
        }

        // Center over pad
        float padCenterX = nearestPad.x + nearestPad.width / 2;
        player.position.x += (padCenterX - player.position.x) * 0.02f;
    }
}

void UpdateGame() {
    if (currentState == GAME) {
        gameTime += GetFrameTime();

        if (IsKeyPressed(KEY_A)) {
            assistMode = !assistMode;
        }

        if (IsKeyDown(KEY_LEFT)) player.rotation -= ROTATION_SPEED;
        if (IsKeyDown(KEY_RIGHT)) player.rotation += ROTATION_SPEED;

        if (IsKeyDown(KEY_UP) && player.fuel > 0) {
            player.velocity.x += std::sin(player.rotation * DEG2RAD) * THRUST;
            player.velocity.y -= std::cos(player.rotation * DEG2RAD) * THRUST;
            player.fuel -= FUEL_CONSUMPTION * GetFrameTime();


            // Calculate the bottom position of the ship
 // Calculate the two bottom points of the ship
            Vector2 bottomPoint1 = {
                player.position.x + std::sin((player.rotation + 135) * DEG2RAD) * PLAYER_SIZE,
                player.position.y - std::cos((player.rotation + 135) * DEG2RAD) * PLAYER_SIZE
            };

            Vector2 bottomPoint2 = {
                player.position.x + std::sin((player.rotation + 225) * DEG2RAD) * PLAYER_SIZE,
                player.position.y - std::cos((player.rotation + 225) * DEG2RAD) * PLAYER_SIZE
            };

            // Randomly pick a point between bottomPoint1 and bottomPoint2
            float t = RandomNumber(0.0, 1.0, 2, false); // t is a random value between 0 and 1
            Vector2 particlePosition = {
                bottomPoint1.x + t * (bottomPoint2.x - bottomPoint1.x),
                bottomPoint1.y + t * (bottomPoint2.y - bottomPoint1.y)
            };

            // Calculate the particle velocity (opposite to the thrust direction)
            Vector2 particleVelocity = {
                -std::sin(player.rotation * DEG2RAD) * RandomNumber(1.0f, 2.0f, 2, false),
                std::cos(player.rotation * DEG2RAD) * RandomNumber(1.0f, 2.0f, 2, false)
            };

            // Create exhaust particles
            Particle particle;
            particle.position = particlePosition;
            particle.velocity = particleVelocity;
            particle.alpha = 1.0f;
            particle.size = RandomNumber(2.0f, 4.0f, 1, false);

            exhaustParticles.push_back(particle);
        }
        player.velocity.y += GRAVITY;
        player.position.x += player.velocity.x;
        player.position.y += player.velocity.y;

        if (assistMode) {
            ApplyAssist();
        }

        // Update particles
        for (auto& particle : exhaustParticles) {
            particle.position.x += particle.velocity.x;
            particle.position.y += particle.velocity.y;
            particle.alpha -= 0.02f; // Fade out
        }

        // Remove particles that are invisible
        exhaustParticles.erase(std::remove_if(exhaustParticles.begin(), exhaustParticles.end(),
            [](const Particle& p) { return p.alpha <= 0; }), exhaustParticles.end());


        // Calculate lander points for collision detection
        Vector2 v1 = { player.position.x + std::sin((player.rotation - 150) * DEG2RAD) * PLAYER_SIZE,
                       player.position.y - std::cos((player.rotation - 150) * DEG2RAD) * PLAYER_SIZE };
        Vector2 v2 = { player.position.x + std::sin((player.rotation + 150) * DEG2RAD) * PLAYER_SIZE,
                       player.position.y - std::cos((player.rotation + 150) * DEG2RAD) * PLAYER_SIZE };
        Vector2 v3 = { player.position.x + std::sin(player.rotation * DEG2RAD) * PLAYER_SIZE * 1.5f,
                       player.position.y - std::cos(player.rotation * DEG2RAD) * PLAYER_SIZE * 1.5f };

        bool collision = CheckLanderCollision(v1, v2, v3);
        bool successfulLanding = CheckSuccessfulLanding(v1, v2, v3);

        if (collision || successfulLanding) {
            currentState = GAME_OVER;
            if (successfulLanding) {
                score = static_cast<int>(1000 - gameTime * 10 + player.fuel * 10);
                if (score < 0) score = 0;
                collisionReason = LANDING_PAD_SUCCESS;
            }
            else {
                score = 0;
                for (const auto& pad : landingPads) {
                    if (CheckCollisionPointRec(v1, pad) || CheckCollisionPointRec(v2, pad) || CheckCollisionPointRec(v3, pad)) {
                        collisionReason = LANDING_PAD_FAIL;
                        return;
                    }
                }
                collisionReason = TERRAIN;
            }
        }

        // Wrap around screen edges
        if (player.position.x > GetScreenWidth()) player.position.x = 0;
        if (player.position.x < 0) player.position.x = static_cast<float>(GetScreenWidth());
    }

    if (currentState == SPLASH && IsKeyPressed(KEY_ENTER)) {
        currentState = GAME;
        InitGame();
    }

    if (currentState == GAME_OVER && IsKeyPressed(KEY_ENTER)) {
        currentState = GAME;
        InitGame();
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(BLACK);

    if (currentState == SPLASH) {
        DrawText("LUNAR LANDER", GetScreenWidth() / 2 - MeasureText("LUNAR LANDER", 40) / 2, GetScreenHeight() / 3, 40, YELLOW);
        DrawText("Press ENTER to start", GetScreenWidth() / 2 - MeasureText("Press ENTER to start", 20) / 2, GetScreenHeight() / 2, 20, WHITE);
        DrawText("Press A to toggle assist mode", GetScreenWidth() / 2 - MeasureText("Press A to toggle assist mode", 20) / 2, GetScreenHeight() / 2 + 30, 20, WHITE);
    }
    else if (currentState == GAME || currentState == GAME_OVER) {
        // Draw terrain
        for (size_t i = 0; i < terrain.size() - 1; ++i) {
            DrawLineV(terrain[i], terrain[i + 1], WHITE);
        }

        // Draw landing pads
        for (const auto& pad : landingPads) {
            DrawRectangleRec(pad, GREEN);
        }

        // Draw exhaust particles
        for (const auto& particle : exhaustParticles) {
            Color color = { 255, 255, 255, static_cast<unsigned char>(particle.alpha * 255) };
            DrawCircleV(particle.position, particle.size, color);
        }

        // Draw player
        Vector2 v1 = { player.position.x + std::sin((player.rotation - 150) * DEG2RAD) * PLAYER_SIZE,
                      player.position.y - std::cos((player.rotation - 150) * DEG2RAD) * PLAYER_SIZE };
        Vector2 v2 = { player.position.x + std::sin((player.rotation + 150) * DEG2RAD) * PLAYER_SIZE,
                      player.position.y - std::cos((player.rotation + 150) * DEG2RAD) * PLAYER_SIZE };
        Vector2 v3 = { player.position.x + std::sin(player.rotation * DEG2RAD) * PLAYER_SIZE * 1.5f,
                      player.position.y - std::cos(player.rotation * DEG2RAD) * PLAYER_SIZE * 1.5f };

        DrawTriangle(v1, v2, v3, WHITE);

        // Draw velocity indicator
        float velocity = Vector2Length(player.velocity);
        Color velocityColor = (velocity < 0.2f) ? GREEN : (velocity < 0.4f) ? YELLOW : RED;
        DrawText(TextFormat("%.2f", velocity), player.position.x - 20, player.position.y - 40, 20, velocityColor);

        // Draw angle indicator
        float angle = std::abs(player.rotation);
        Color angleColor = (angle < 5.0f) ? GREEN : (angle < 10.0f) ? YELLOW : RED;
        DrawText(TextFormat("%.1f", angle), player.position.x + 20, player.position.y - 40, 20, angleColor);

        // Draw HUD
        DrawText(TextFormat("SCORE: %d", score), 10, 10, 20, WHITE);
        DrawText(TextFormat("FUEL: %.0f", player.fuel), 10, 40, 20, WHITE);
        DrawText(TextFormat("ASSIST: %s", assistMode ? "ON" : "OFF"), 10, 70, 20, assistMode ? GREEN : RED);

        // Draw collision reason
        std::string reasonText;
        Color reasonColor;
        switch (collisionReason) {
        case TERRAIN:
            reasonText = "COLLISION: TERRAIN";
            reasonColor = RED;
            break;
        case LANDING_PAD_SUCCESS:
            reasonText = "LANDED SUCCESSFULLY";
            reasonColor = GREEN;
            break;
        case LANDING_PAD_FAIL:
            reasonText = "LANDING FAILED";
            reasonColor = YELLOW;
            break;
        default:
            reasonText = "";
            reasonColor = WHITE;
        }
        if (!reasonText.empty()) {
            DrawText(reasonText.c_str(), 10, 100, 20, reasonColor);
        }

        if (currentState == GAME_OVER) {
            const char* message = score > 0 ? "LANDED!" : "CRASHED!";
            DrawText(message, GetScreenWidth() / 2 - MeasureText(message, 40) / 2, GetScreenHeight() / 3, 40, score > 0 ? GREEN : RED);
            DrawText(TextFormat("SCORE: %d", score), GetScreenWidth() / 2 - MeasureText(TextFormat("SCORE: %d", score), 30) / 2, GetScreenHeight() / 2, 30, WHITE);
            DrawText("Press ENTER to retry", GetScreenWidth() / 2 - MeasureText("Press ENTER to retry", 20) / 2, GetScreenHeight() * 2 / 3, 20, WHITE);
        }
    }

    EndDrawing();
}
