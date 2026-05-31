#include "game.h"

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    Texture2D logoTexture = LoadTexture("BolaFora-white.png");

    Player ship = { 0 };
    Bullet* bulletsHead = NULL;
    Asteroid asteroids[MAX_ASTEROIDS] = { 0 };
    Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER] = { 0 };

    Enemy ufos[MAX_UFOS] = { 0 };
    Boss boss = { 0 };

    int score = 0;
    int highScore = LoadHighScore();
    float shootCooldown = 0.0f;
    int level = 1;

    GameScreen currentScreen = SPLASH;
    float splashTimer = 4.5f;

    ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateStarfield(starfield);

        if (currentScreen == GAMEPLAY || currentScreen == ENDING) {
            UpdateAsteroids(asteroids);
        }

        if (currentScreen == SPLASH) {
            splashTimer -= GetFrameTime();

            if (splashTimer <= 0.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {

                ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);
                currentScreen = MENU;
            }
        } else {
            switch (currentScreen) {
            case MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);
                    currentScreen = GAMEPLAY;
                }
                break;

            case GAMEPLAY:
                UpdatePlayer(&ship);
                UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, false);
                UpdateBoss(&boss, &ship, &bulletsHead, false);
                UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, false);
                CheckLevelClear(asteroids, &level, &ship, &bulletsHead, ufos, &boss);

                bool playerHit = false;

                if (ship.invulnerableTimer <= 0.0f) {
                    for (int i = 0; i < MAX_ASTEROIDS; i++) {
                        if (asteroids[i].active && CheckCollisionCircles(ship.position, ship.size * 0.6f, asteroids[i].position, asteroids[i].radius)) {
                            playerHit = true;
                        }
                    }

                    for (int e = 0; e < MAX_UFOS; e++) {
                        if (ufos[e].active && CheckCollisionCircles(ship.position, ship.size * 0.6f, ufos[e].position, ufos[e].radius)) {
                            playerHit = true;
                        }
                    }

                    if (boss.active && CheckCollisionCircles(ship.position, ship.size * 0.6f, boss.position, boss.radius * 0.8f)) {
                        playerHit = true;
                    }

                    Bullet* cb = bulletsHead;
                    while (cb != NULL) {
                        if (cb->isEnemy && CheckCollisionCircles(ship.position, ship.size * 0.6f, cb->position, 2.0f)) {
                            playerHit = true;
                        }
                        cb = cb->next;
                    }
                }

                if (playerHit) {
                    ship.lives--;

                    if (ship.lives <= 0) {
                        currentScreen = ENDING;

                        if (score > highScore) {
                            highScore = score;
                            SaveHighScore(highScore);
                        }
                    } else {
                        ship.position = (Vector2) { screenWidth / 2.0f, screenHeight / 2.0f };
                        ship.velocity = (Vector2) { 0, 0 };
                        ship.invulnerableTimer = 2.0f;
                    }
                }
                break;

            case ENDING:
                UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, true);
                UpdateBoss(&boss, &ship, &bulletsHead, true);
                UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, true);

                if (IsKeyPressed(KEY_ENTER)) {
                    ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);
                    currentScreen = MENU;
                }
                break;
            }
        }

        DrawGame(&ship, bulletsHead, asteroids, ufos, &boss, score, highScore, level, currentScreen, starfield, logoTexture, splashTimer);
    }

    UnloadTexture(logoTexture);

    Bullet* currentBullet = bulletsHead;
    while (currentBullet != NULL) {
        Bullet* nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }

    CloseWindow();
    return 0;
}