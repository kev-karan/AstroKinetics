#include "game.h"

int LoadHighScore(void)
{
    FILE* file = fopen("topscore.txt", "r");
    int hs = 0;
    if (file != NULL) {
        fscanf(file, "%d", &hs);
        fclose(file);
    }
    return hs;
}

void SaveHighScore(int score)
{
    FILE* file = fopen("topscore.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d", score);
        fclose(file);
    }
}

void ResetGame(Player* ship, Bullet** bulletsHead, Asteroid* asteroids, Enemy* ufo, Boss* boss, int* score, int* level, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER])
{
    ship->position = (Vector2) { screenWidth / 2.0f, screenHeight / 2.0f };
    ship->velocity = (Vector2) { 0, 0 };
    ship->rotation = 0.0f;
    ship->size = 20.0f;
    ship->color = RAYWHITE;

    Bullet* currentBullet = *bulletsHead;
    while (currentBullet != NULL) {
        Bullet* nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }
    *bulletsHead = NULL;

    ufo->active = false;
    boss->active = false;
    boss->introTimer = 0.0f;

    for (int i = 0; i < MAX_ASTEROIDS; i++)
        asteroids[i].active = false;

    int initialAsteroids = 4;
    for (int i = 0; i < initialAsteroids; i++) {
        asteroids[i].active = true;
        asteroids[i].radius = 40.0f;

        for (int v = 0; v < ASTEROID_VERTICES; v++) {
            asteroids[i].vertexOffsets[v] = GetRandomValue(80, 120) / 100.0f;
        }

        do {
            asteroids[i].position.x = GetRandomValue(0, screenWidth);
            asteroids[i].position.y = GetRandomValue(0, screenHeight);
        } while (CheckCollisionCircles(asteroids[i].position, 40.0f, ship->position, 100.0f));

        asteroids[i].velocity.x = GetRandomValue(-200, 200) / 100.0f;
        asteroids[i].velocity.y = GetRandomValue(-200, 200) / 100.0f;
    }

    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < STARS_PER_LAYER; j++) {
            starfield[i][j].x = GetRandomValue(0, screenWidth);
            starfield[i][j].y = GetRandomValue(0, screenHeight);
        }
    }

    *score = 0;

    // --- MODO DE TESTE ---
    *level = 4;
    // ---------------------
}