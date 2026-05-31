#include "game.h"

void LoadHighScores(HighScoreEntry* scores)
{
    FILE* file = fopen("topscores.txt", "r");
    bool loaded = false;

    if (file != NULL) {
        int count = 0;
        for (int i = 0; i < MAX_HIGH_SCORES; i++) {
            if (fscanf(file, "%3s %d", scores[i].name, &scores[i].score) == 2) {
                count++;
            }
        }
        fclose(file);
        if (count == MAX_HIGH_SCORES)
            loaded = true;
    }

    if (!loaded) {
        for (int i = 0; i < MAX_HIGH_SCORES; i++) {
            sprintf(scores[i].name, "CPU");
            scores[i].score = (MAX_HIGH_SCORES - i) * 1000;
        }
    }
}

void SaveHighScores(HighScoreEntry* scores)
{
    FILE* file = fopen("topscores.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < MAX_HIGH_SCORES; i++) {
            fprintf(file, "%s %d\n", scores[i].name, scores[i].score);
        }
        fclose(file);
    }
}

void ResetGame(Player* ship, Bullet** bulletsHead, Asteroid* asteroids, Enemy* ufos, Boss* boss, int* score, int* level, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER], Particle* particles)
{
    ship->position = (Vector2) { screenWidth / 2.0f, screenHeight / 2.0f };
    ship->velocity = (Vector2) { 0, 0 };
    ship->rotation = 0.0f;
    ship->size = 20.0f;
    ship->color = RAYWHITE;
    ship->lives = 5;
    ship->invulnerableTimer = 0.0f;

    Bullet* currentBullet = *bulletsHead;
    while (currentBullet != NULL) {
        Bullet* nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }
    *bulletsHead = NULL;

    for (int e = 0; e < MAX_UFOS; e++) {
        ufos[e].active = false;
    }

    for (int p = 0; p < MAX_PARTICLES; p++) {
        particles[p].active = false;
    }

    boss->active = false;
    boss->introTimer = 0.0f;
    boss->bounceCount = 0;
    boss->isCrossing = false;

    for (int i = 0; i < MAX_ASTEROIDS; i++)
        asteroids[i].active = false;

    int initialAsteroids = 3;
    for (int i = 0; i < initialAsteroids; i++) {
        asteroids[i].active = true;
        asteroids[i].radius = 40.0f;

        asteroids[i].rotation = GetRandomValue(0, 360);
        asteroids[i].rotationSpeed = GetRandomValue(-200, 200) / 100.0f;

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

    *score = 0;
    *level = 1;
}