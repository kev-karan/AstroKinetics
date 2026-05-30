#include "game.h"

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    Player ship = { 0 };
    Bullet* bulletsHead = NULL;
    Asteroid asteroids[MAX_ASTEROIDS] = { 0 };
    Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER] = { 0 };

    int score = 0;
    int highScore = LoadHighScore();
    float shootCooldown = 0.0f;

    GameScreen currentScreen = MENU;

    ResetGame(&ship, &bulletsHead, asteroids, &score, starfield);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateAsteroids(asteroids);
        UpdateStarfield(starfield);

        switch (currentScreen) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame(&ship, &bulletsHead, asteroids, &score, starfield);
                currentScreen = GAMEPLAY;
            }
            break;

        case GAMEPLAY:
            UpdatePlayer(&ship);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, &score);

            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (asteroids[i].active) {
                    if (CheckCollisionCircles(ship.position, ship.size * 0.6f, asteroids[i].position, asteroids[i].radius)) {
                        currentScreen = ENDING;
                        if (score > highScore) {
                            highScore = score;
                            SaveHighScore(highScore);
                        }
                    }
                }
            }
            break;

        case ENDING:
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = MENU;
            }
            break;
        }
        DrawGame(&ship, bulletsHead, asteroids, score, highScore, currentScreen, starfield);
    }

    Bullet* currentBullet = bulletsHead;
    while (currentBullet != NULL) {
        Bullet* nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }

    CloseWindow();
    return 0;
}