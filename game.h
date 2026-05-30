#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// --- CONSTANTES ---
#define MAX_ASTEROIDS 40
#define ASTEROID_VERTICES 10
#define NUM_LAYERS 3
#define STARS_PER_LAYER 100

// --- TELA ---
static const int screenWidth = 800;
static const int screenHeight = 600;

// --- ESTRUTURAS ---
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
    bool isEnemy;
    struct Bullet* next;
} Bullet;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    float vertexOffsets[ASTEROID_VERTICES];
} Asteroid;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    float shootTimer;
} Enemy;

typedef enum GameScreen { MENU,
    GAMEPLAY,
    ENDING } GameScreen;

// --- logic.c ---
void UpdatePlayer(Player* ship);
void UpdateBullets(Bullet** bulletsHead, Player* ship, float* shootCooldown, Asteroid* asteroids, Enemy* ufo, int* score);
void UpdateEnemy(Enemy* ufo, Player* ship, Bullet** bulletsHead);
void UpdateAsteroids(Asteroid* asteroids);
void UpdateStarfield(Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER]);
void CheckLevelClear(Asteroid* asteroids, int* level, Player* ship, Bullet** bulletsHead, Enemy* ufo);

// --- graphics.c ---
void DrawGame(Player* ship, Bullet* bulletsHead, Asteroid* asteroids, Enemy* ufo, int score, int highScore, int level, GameScreen currentScreen, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER]);

// --- utils.c ---
void ResetGame(Player* ship, Bullet** bulletsHead, Asteroid* asteroids, Enemy* ufo, int* score, int* level, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER]);
int LoadHighScore(void);
void SaveHighScore(int score);

#endif