#include "game.h"

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    Player ship = { 0 };
    Bullet* bulletsHead = NULL;
    Asteroid asteroids[MAX_ASTEROIDS] = { 0 };
    Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER] = { 0 };

    Enemy ufo = { 0 };

    int score = 0;
    int highScore = LoadHighScore();
    float shootCooldown = 0.0f;
    int level = 1;

    GameScreen currentScreen = MENU;

    ResetGame(&ship, &bulletsHead, asteroids, &ufo, &score, &level, starfield);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateAsteroids(asteroids);
        UpdateStarfield(starfield);

        switch (currentScreen) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame(&ship, &bulletsHead, asteroids, &ufo, &score, &level, starfield);
                currentScreen = GAMEPLAY;
            }
            break;

        case GAMEPLAY:
            UpdatePlayer(&ship);
            UpdateEnemy(&ufo, &ship, &bulletsHead); // Movimenta e atira com o UFO
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, &ufo, &score);
            CheckLevelClear(asteroids, &level, &ship, &bulletsHead, &ufo);

            bool playerHit = false;

            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (asteroids[i].active && CheckCollisionCircles(ship.position, ship.size * 0.6f, asteroids[i].position, asteroids[i].radius)) {
                    playerHit = true;
                }
            }

            if (ufo.active && CheckCollisionCircles(ship.position, ship.size * 0.6f, ufo.position, ufo.radius)) {
                playerHit = true;
            }

            Bullet* cb = bulletsHead;
            while (cb != NULL) {
                if (cb->isEnemy && CheckCollisionCircles(ship.position, ship.size * 0.6f, cb->position, 2.0f)) {
                    playerHit = true;
                }
                cb = cb->next;
            }

            if (playerHit) {
                currentScreen = ENDING;
                if (score > highScore) {
                    highScore = score;
                    SaveHighScore(highScore);
                }
            }
            break;

        case ENDING:
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = MENU;
            }
            break;
        }

        DrawGame(&ship, bulletsHead, asteroids, &ufo, score, highScore, level, currentScreen, starfield);
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