#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ASTEROIDS 40
#define ASTEROID_VERTICES 10

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float size;
    Color color;
} Player;

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    float lifeTime;
    struct Bullet* next;
} Bullet;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    float vertexOffsets[ASTEROID_VERTICES];
} Asteroid;

typedef enum GameScreen { MENU,
    GAMEPLAY,
    ENDING } GameScreen;

const int screenWidth = 800;
const int screenHeight = 600;

void UpdatePlayer(Player* ship);
void UpdateBullets(Bullet** bulletsHead, Player* ship, float* shootCooldown, Asteroid* asteroids, int* score);
void UpdateAsteroids(Asteroid* asteroids);
void DrawGame(Player* ship, Bullet* bulletsHead, Asteroid* asteroids, int score, int highScore, GameScreen currentScreen);
void ResetGame(Player* ship, Bullet** bulletsHead, Asteroid* asteroids, int* score);
int LoadHighScore(void);
void SaveHighScore(int score);

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    Player ship = { 0 };
    Bullet* bulletsHead = NULL;
    Asteroid asteroids[MAX_ASTEROIDS] = { 0 };

    int score = 0;
    int highScore = LoadHighScore();
    float shootCooldown = 0.0f;

    GameScreen currentScreen = MENU;

    ResetGame(&ship, &bulletsHead, asteroids, &score);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateAsteroids(asteroids);

        switch (currentScreen) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame(&ship, &bulletsHead, asteroids, &score);
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

        DrawGame(&ship, bulletsHead, asteroids, score, highScore, currentScreen);
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

void ResetGame(Player* ship, Bullet** bulletsHead, Asteroid* asteroids, int* score)
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

    for (int i = 0; i < MAX_ASTEROIDS; i++)
        asteroids[i].active = false;

    int initialAsteroids = 4;
    for (int i = 0; i < initialAsteroids; i++) {
        asteroids[i].active = true;
        asteroids[i].radius = 40.0f;
        do {
            asteroids[i].position.x = GetRandomValue(0, screenWidth);
            asteroids[i].position.y = GetRandomValue(0, screenHeight);
        } while (CheckCollisionCircles(asteroids[i].position, 40.0f, ship->position, 100.0f));

        asteroids[i].velocity.x = GetRandomValue(-200, 200) / 100.0f;
        asteroids[i].velocity.y = GetRandomValue(-200, 200) / 100.0f;
    }

    *score = 0;
}

void UpdatePlayer(Player* ship)
{
    if (IsKeyDown(KEY_RIGHT))
        ship->rotation += 5.0f;
    if (IsKeyDown(KEY_LEFT))
        ship->rotation -= 5.0f;

    if (IsKeyDown(KEY_UP)) {
        float rotationInRadians = (ship->rotation - 90) * DEG2RAD;
        ship->velocity.x += cosf(rotationInRadians) * 0.15f;
        ship->velocity.y += sinf(rotationInRadians) * 0.15f;
    }

    ship->velocity.x *= 0.99f;
    ship->velocity.y *= 0.99f;

    float maxSpeed = 6.0f;
    float currentSpeed = sqrtf(ship->velocity.x * ship->velocity.x + ship->velocity.y * ship->velocity.y);

    if (currentSpeed > maxSpeed) {
        ship->velocity.x = (ship->velocity.x / currentSpeed) * maxSpeed;
        ship->velocity.y = (ship->velocity.y / currentSpeed) * maxSpeed;
    }

    ship->position.x += ship->velocity.x;
    ship->position.y += ship->velocity.y;

    if (ship->position.x > screenWidth + ship->size)
        ship->position.x = -ship->size;
    else if (ship->position.x < -ship->size)
        ship->position.x = screenWidth + ship->size;

    if (ship->position.y > screenHeight + ship->size)
        ship->position.y = -ship->size;
    else if (ship->position.y < -ship->size)
        ship->position.y = screenHeight + ship->size;
}

void UpdateBullets(Bullet** bulletsHead, Player* ship, float* shootCooldown, Asteroid* asteroids, int* score)
{
    float baseAngle = ship->rotation - 90.0f;

    if (*shootCooldown > 0.0f)
        *shootCooldown -= GetFrameTime();

    if (IsKeyDown(KEY_SPACE) && *shootCooldown <= 0.0f) {
        Bullet* newBullet = (Bullet*)malloc(sizeof(Bullet));

        newBullet->position.x = ship->position.x + cosf(baseAngle * DEG2RAD) * ship->size;
        newBullet->position.y = ship->position.y + sinf(baseAngle * DEG2RAD) * ship->size;

        float bulletSpeed = 10.0f;
        newBullet->velocity.x = cosf(baseAngle * DEG2RAD) * bulletSpeed;
        newBullet->velocity.y = sinf(baseAngle * DEG2RAD) * bulletSpeed;
        newBullet->lifeTime = 0.9f;

        newBullet->next = *bulletsHead;
        *bulletsHead = newBullet;
        *shootCooldown = 0.2f;
    }

    Bullet* currentBullet = *bulletsHead;
    Bullet* previousBullet = NULL;

    while (currentBullet != NULL) {
        currentBullet->position.x += currentBullet->velocity.x;
        currentBullet->position.y += currentBullet->velocity.y;
        currentBullet->lifeTime -= GetFrameTime();

        if (currentBullet->position.x > screenWidth)
            currentBullet->position.x = 0;
        else if (currentBullet->position.x < 0)
            currentBullet->position.x = screenWidth;
        if (currentBullet->position.y > screenHeight)
            currentBullet->position.y = 0;
        else if (currentBullet->position.y < 0)
            currentBullet->position.y = screenHeight;

        bool bulletHit = false;
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                if (CheckCollisionCircles(currentBullet->position, 2.0f, asteroids[i].position, asteroids[i].radius)) {
                    bulletHit = true;
                    asteroids[i].active = false;

                    if (asteroids[i].radius >= 40.0f) {
                        *score += 50;
                    } else {
                        *score += 100;
                    }

                    if (asteroids[i].radius >= 20.0f) {
                        float newRadius = asteroids[i].radius / 2.0f;
                        int spawned = 0;

                        for (int j = 0; j < MAX_ASTEROIDS && spawned < 2; j++) {
                            if (!asteroids[j].active) {
                                asteroids[j].active = true;

                                float offset = (spawned == 0) ? -newRadius : newRadius;
                                asteroids[j].position.x = asteroids[i].position.x + offset;
                                asteroids[j].position.y = asteroids[i].position.y + offset;

                                asteroids[j].radius = newRadius;
                                asteroids[j].velocity.x = GetRandomValue(-300, 300) / 100.0f;
                                asteroids[j].velocity.y = GetRandomValue(-300, 300) / 100.0f;
                                spawned++;
                            }
                        }
                    }
                    break;
                }
            }
        }

        if (bulletHit)
            currentBullet->lifeTime = 0.0f;

        if (currentBullet->lifeTime <= 0.0f) {
            Bullet* toDelete = currentBullet;
            if (previousBullet == NULL) {
                *bulletsHead = currentBullet->next;
                currentBullet = *bulletsHead;
            } else {
                previousBullet->next = currentBullet->next;
                currentBullet = previousBullet->next;
            }
            free(toDelete);
        } else {
            previousBullet = currentBullet;
            currentBullet = currentBullet->next;
        }
    }
}

void UpdateAsteroids(Asteroid* asteroids)
{
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroids[i].active)
            continue;

        for (int j = i + 1; j < MAX_ASTEROIDS; j++) {
            if (!asteroids[j].active)
                continue;

            float dx = asteroids[i].position.x - asteroids[j].position.x;
            float dy = asteroids[i].position.y - asteroids[j].position.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance == 0.0f) {
                dx = 0.1f;
                distance = 0.1f;
            }

            float sumRadii = asteroids[i].radius + asteroids[j].radius;
            if (distance < sumRadii) {
                float nx = dx / distance;
                float ny = dy / distance;

                float overlap = sumRadii - distance;
                asteroids[i].position.x += nx * (overlap / 2.0f);
                asteroids[i].position.y += ny * (overlap / 2.0f);
                asteroids[j].position.x -= nx * (overlap / 2.0f);
                asteroids[j].position.y -= ny * (overlap / 2.0f);

                float dvx = asteroids[i].velocity.x - asteroids[j].velocity.x;
                float dvy = asteroids[i].velocity.y - asteroids[j].velocity.y;

                float dotProduct = (dvx * nx) + (dvy * ny);

                if (dotProduct < 0) {
                    float m1 = asteroids[i].radius;
                    float m2 = asteroids[j].radius;

                    float impulse = (2.0f * dotProduct) / (m1 + m2);

                    asteroids[i].velocity.x -= impulse * m2 * nx;
                    asteroids[i].velocity.y -= impulse * m2 * ny;
                    asteroids[j].velocity.x += impulse * m1 * nx;
                    asteroids[j].velocity.y += impulse * m1 * ny;
                }
            }
        }
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            asteroids[i].position.x += asteroids[i].velocity.x;
            asteroids[i].position.y += asteroids[i].velocity.y;

            if (asteroids[i].position.x > screenWidth + asteroids[i].radius)
                asteroids[i].position.x = -asteroids[i].radius;
            else if (asteroids[i].position.x < -asteroids[i].radius)
                asteroids[i].position.x = screenWidth + asteroids[i].radius;

            if (asteroids[i].position.y > screenHeight + asteroids[i].radius)
                asteroids[i].position.y = -asteroids[i].radius;
            else if (asteroids[i].position.y < -asteroids[i].radius)
                asteroids[i].position.y = screenHeight + asteroids[i].radius;
        }
    }
}

void DrawGame(Player* ship, Bullet* bulletsHead, Asteroid* asteroids, int score, int highScore, GameScreen currentScreen)
{
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            DrawCircleLines(asteroids[i].position.x, asteroids[i].position.y, asteroids[i].radius, RAYWHITE);
        }
    }

    if (currentScreen != MENU) {
        Bullet* currentBullet = bulletsHead;
        while (currentBullet != NULL) {
            DrawCircleV(currentBullet->position, 2.0f, WHITE);
            currentBullet = currentBullet->next;
        }
    }

    switch (currentScreen) {
    case MENU: {
        int titleWidth = MeasureText("AstroKinetics", 60);
        DrawText("AstroKinetics", (screenWidth / 2) - (titleWidth / 2), screenHeight / 2 - 80, 60, RAYWHITE);

        int startWidth = MeasureText("PRESS ENTER TO START", 20);
        DrawText("PRESS ENTER TO START", (screenWidth / 2) - (startWidth / 2), screenHeight / 2 + 20, 20, GRAY);

        int hsWidth = MeasureText(TextFormat("HI-SCORE: %05i", highScore), 20);
        DrawText(TextFormat("HI-SCORE: %05i", highScore), (screenWidth / 2) - (hsWidth / 2), screenHeight / 2 + 80, 20, DARKGRAY);
    } break;

    case GAMEPLAY: {
        float baseAngle = ship->rotation - 90.0f;
        Vector2 p1 = { ship->position.x + cosf(baseAngle * DEG2RAD) * ship->size, ship->position.y + sinf(baseAngle * DEG2RAD) * ship->size };
        Vector2 p2 = { ship->position.x + cosf((baseAngle + 140.0f) * DEG2RAD) * ship->size, ship->position.y + sinf((baseAngle + 140.0f) * DEG2RAD) * ship->size };
        Vector2 p3 = { ship->position.x + cosf((baseAngle - 140.0f) * DEG2RAD) * ship->size, ship->position.y + sinf((baseAngle - 140.0f) * DEG2RAD) * ship->size };
        DrawTriangleLines(p1, p2, p3, ship->color);

        DrawText(TextFormat("SCORE: %05i", score), 20, 20, 30, RAYWHITE);
        DrawText(TextFormat("HI-SCORE: %05i", highScore), 20, 60, 20, GRAY);
    } break;

    case ENDING: {
        DrawText(TextFormat("SCORE: %05i", score), 20, 20, 30, RAYWHITE);
        DrawText(TextFormat("HI-SCORE: %05i", highScore), 20, 60, 20, GRAY);

        int gameOverWidth = MeasureText("GAME OVER", 60);
        DrawText("GAME OVER", (screenWidth / 2) - (gameOverWidth / 2), screenHeight / 2 - 50, 60, RED);

        int restartWidth = MeasureText("PRESS ENTER TO RETURN TO MENU", 20);
        DrawText("PRESS ENTER TO RETURN TO MENU", (screenWidth / 2) - (restartWidth / 2), screenHeight / 2 + 30, 20, GRAY);
    } break;
    }

    EndDrawing();
}