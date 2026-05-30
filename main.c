#include "game.h"

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    Player ship = { 0 };
    Bullet* bulletsHead = NULL;
    Asteroid asteroids[MAX_ASTEROIDS] = { 0 };
    Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER] = { 0 };

    Enemy ufo = { 0 };
    Boss boss = { 0 };

    int score = 0;
    int highScore = LoadHighScore();
    float shootCooldown = 0.0f;
    int level = 1;

    GameScreen currentScreen = MENU;

    ResetGame(&ship, &bulletsHead, asteroids, &ufo, &boss, &score, &level, starfield);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateAsteroids(asteroids);
        UpdateStarfield(starfield);

        switch (currentScreen) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame(&ship, &bulletsHead, asteroids, &ufo, &boss, &score, &level, starfield);
                currentScreen = GAMEPLAY;
            }
            break;

        case GAMEPLAY:
            UpdatePlayer(&ship);
            UpdateEnemy(&ufo, &ship, &bulletsHead, asteroids, false);
            UpdateBoss(&boss, &ship, &bulletsHead, false);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, &ufo, &boss, &score, false);

            CheckLevelClear(asteroids, &level, &ship, &bulletsHead, &ufo, &boss);

            bool playerHit = false;

            if (ship.invulnerableTimer <= 0.0f) {
                for (int i = 0; i < MAX_ASTEROIDS; i++) {
                    if (asteroids[i].active && CheckCollisionCircles(ship.position, ship.size * 0.6f, asteroids[i].position, asteroids[i].radius)) {
                        playerHit = true;
                    }
                }

                if (ufo.active && CheckCollisionCircles(ship.position, ship.size * 0.6f, ufo.position, ufo.radius)) {
                    playerHit = true;
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

                Bullet* cbToFree = bulletsHead;
                while (cbToFree != NULL) {
                    Bullet* next = cbToFree->next;
                    free(cbToFree);
                    cbToFree = next;
                }
                bulletsHead = NULL;

                if (ship.lives <= 0) {
                    currentScreen = ENDING;
                    ufo.active = false;

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
            UpdateEnemy(&ufo, &ship, &bulletsHead, asteroids, true);
            UpdateBoss(&boss, &ship, &bulletsHead, true);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, &ufo, &boss, &score, true);

            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame(&ship, &bulletsHead, asteroids, &ufo, &boss, &score, &level, starfield);
                currentScreen = MENU;
            }
            break;
        }

        DrawGame(&ship, bulletsHead, asteroids, &ufo, &boss, score, highScore, level, currentScreen, starfield);
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