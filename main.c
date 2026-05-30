#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

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
} Asteroid;

#define MAX_ASTEROIDS 40

const int screenWidth = 800;
const int screenHeight = 600;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    Player ship = { 0 };
    ship.position = (Vector2) { screenWidth / 2.0f, screenHeight / 2.0f };
    ship.velocity = (Vector2) { 0, 0 };
    ship.rotation = 0.0f;
    ship.size = 20.0f;
    ship.color = RAYWHITE;

    Bullet* bulletsHead = NULL;
    float shootCooldown = 0.0f;

    Asteroid asteroids[MAX_ASTEROIDS] = { 0 };
    int initialAsteroids = 4;

    for (int i = 0; i < initialAsteroids; i++) {
        asteroids[i].active = true;
        asteroids[i].radius = 40.0f;

        asteroids[i].position.x = GetRandomValue(0, screenWidth);
        asteroids[i].position.y = GetRandomValue(0, screenHeight);

        asteroids[i].velocity.x = GetRandomValue(-200, 200) / 100.0f;
        asteroids[i].velocity.y = GetRandomValue(-200, 200) / 100.0f;
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT))
            ship.rotation += 5.0f;
        if (IsKeyDown(KEY_LEFT))
            ship.rotation -= 5.0f;

        if (IsKeyDown(KEY_UP)) {
            float rotationInRadians = (ship.rotation - 90) * DEG2RAD;

            ship.velocity.x += cosf(rotationInRadians) * 0.15f;
            ship.velocity.y += sinf(rotationInRadians) * 0.15f;
        }

        ship.velocity.x *= 0.99f;
        ship.velocity.y *= 0.99f;

        float maxSpeed = 6.0f;
        float currentSpeed = sqrtf(ship.velocity.x * ship.velocity.x + ship.velocity.y * ship.velocity.y);

        if (currentSpeed > maxSpeed) {
            ship.velocity.x = (ship.velocity.x / currentSpeed) * maxSpeed;
            ship.velocity.y = (ship.velocity.y / currentSpeed) * maxSpeed;
        }

        ship.position.x += ship.velocity.x;
        ship.position.y += ship.velocity.y;

        if (ship.position.x > screenWidth + ship.size)
            ship.position.x = -ship.size;
        else if (ship.position.x < -ship.size)
            ship.position.x = screenWidth + ship.size;
        if (ship.position.y > screenHeight + ship.size)
            ship.position.y = -ship.size;
        else if (ship.position.y < -ship.size)
            ship.position.y = screenHeight + ship.size;

        float baseAngle = ship.rotation - 90.0f;

        if (shootCooldown > 0.0f)
            shootCooldown -= GetFrameTime();

        if (IsKeyDown(KEY_SPACE) && shootCooldown <= 0.0f) {

            Bullet* newBullet = (Bullet*)malloc(sizeof(Bullet));

            newBullet->position.x = ship.position.x + cosf(baseAngle * DEG2RAD) * ship.size;
            newBullet->position.y = ship.position.y + sinf(baseAngle * DEG2RAD) * ship.size;

            float bulletSpeed = 10.0f;
            newBullet->velocity.x = cosf(baseAngle * DEG2RAD) * bulletSpeed;
            newBullet->velocity.y = sinf(baseAngle * DEG2RAD) * bulletSpeed;
            newBullet->lifeTime = 0.9f;

            newBullet->next = bulletsHead;
            bulletsHead = newBullet;
            shootCooldown = 0.2f;
        }

        Bullet* currentBullet = bulletsHead;
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

            if (currentBullet->lifeTime <= 0.0f) {

                Bullet* toDelete = currentBullet;

                if (previousBullet == NULL) {
                    bulletsHead = currentBullet->next;
                    currentBullet = bulletsHead;
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

        BeginDrawing();
        ClearBackground(BLACK);

        currentBullet = bulletsHead;
        while (currentBullet != NULL) {
            DrawCircleV(currentBullet->position, 2.0f, WHITE);
            currentBullet = currentBullet->next;
        }

        Vector2 p1 = {
            ship.position.x + cosf(baseAngle * DEG2RAD) * ship.size,
            ship.position.y + sinf(baseAngle * DEG2RAD) * ship.size
        };

        Vector2 p2 = {
            ship.position.x + cosf((baseAngle + 140.0f) * DEG2RAD) * ship.size,
            ship.position.y + sinf((baseAngle + 140.0f) * DEG2RAD) * ship.size
        };

        Vector2 p3 = {
            ship.position.x + cosf((baseAngle - 140.0f) * DEG2RAD) * ship.size,
            ship.position.y + sinf((baseAngle - 140.0f) * DEG2RAD) * ship.size
        };

        DrawTriangleLines(p1, p2, p3, ship.color);

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                DrawCircleLines(asteroids[i].position.x, asteroids[i].position.y, asteroids[i].radius, RAYWHITE);
            }
        }

        EndDrawing();
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