#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// --- CONSTANTES ---
#define MAX_ASTEROIDS 40
#define MAX_UFOS 3
#define ASTEROID_VERTICES 10
#define NUM_LAYERS 3
#define STARS_PER_LAYER 100
#define MAX_PARTICLES 200
#define MAX_HIGH_SCORES 5

// --- TELA ---
static const int screenWidth = 800;
static const int screenHeight = 600;

// --- ESTRUTURAS ---
typedef struct {
    char name[4];
    int score;
} HighScoreEntry;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float size;
    Color color;
    int lives;
    float invulnerableTimer;
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
    float rotation;
    float rotationSpeed;
} Asteroid;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    float shootTimer;
} Enemy;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    float shootTimer;
    int health;
    int maxHealth;
    float introTimer;
    int bounceCount;
    bool isCrossing;
} Boss;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float lifeTime;
    bool active;
} Particle;

typedef struct {
    Sound shoot;
    Sound explosion;
    Sound enemyShoot;
    Sound enemyExplosion;
    Sound bossHit;
    Sound playerDeath;
    Sound levelUp;
    Sound select;
} GameSounds;

typedef enum GameScreen { SPLASH,
    MENU,
    GAMEPLAY,
    NAME_ENTRY,
    TOP_SCORES,
    ENDING
} GameScreen;

// --- logic.c ---
void UpdatePlayer(Player* ship);
void SpawnParticles(Particle* particles, Vector2 position, int count, Color color);
void UpdateParticles(Particle* particles);
void UpdateBullets(Bullet** bulletsHead, Player* ship, float* shootCooldown, Asteroid* asteroids, Enemy* ufos, Boss* boss, int* score, bool isGameOver, GameSounds* fx, Particle* particles);
void UpdateEnemy(Enemy* ufos, Player* ship, Bullet** bulletsHead, Asteroid* asteroids, bool isGameOver, GameSounds* fx);
void UpdateBoss(Boss* boss, Player* ship, Bullet** bulletsHead, bool isGameOver, GameSounds* fx);
void UpdateAsteroids(Asteroid* asteroids);
void UpdateStarfield(Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER], float hyperspaceTimer);
void CheckLevelClear(Asteroid* asteroids, int* level, Player* ship, Bullet** bulletsHead, Enemy* ufos, Boss* boss, GameSounds* fx, float* hyperspaceTimer, bool* isTransitioning);

// --- graphics.c ---
void DrawGame(Player* ship, Bullet* bulletsHead, Asteroid* asteroids, Enemy* ufos, Boss* boss, int score, HighScoreEntry* highScores, int level, GameScreen currentScreen, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER], Texture2D logoTexture, float splashTimer, Particle* particles, float hyperspaceTimer, char* nameInput, int letterIndex, int frameCounter, int newScoreIndex);

// --- utils.c ---
void ResetGame(Player* ship, Bullet** bulletsHead, Asteroid* asteroids, Enemy* ufos, Boss* boss, int* score, int* level, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER], Particle* particles);
void LoadHighScores(HighScoreEntry* scores);
void SaveHighScores(HighScoreEntry* scores);

#endif