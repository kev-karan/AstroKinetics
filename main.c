#include "game.h"

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    InitAudioDevice();

    Texture2D logoTexture = LoadTexture("BolaFora-white.png");

    Music bgmMenu = LoadMusicStream("IswearIsawit.ogg");
    Music bgmGame = LoadMusicStream("DST-TowerDefenseTheme.mp3");

    PlayMusicStream(bgmMenu);

    GameSounds fx = { 0 };
    fx.shoot = LoadSound("shoot.wav");
    fx.explosion = LoadSound("explosion.wav");
    fx.enemyShoot = LoadSound("enemy_shoot.wav");
    fx.enemyExplosion = LoadSound("enemy_explosion.wav");
    fx.bossHit = LoadSound("boss_hit.wav");
    fx.playerDeath = LoadSound("player_death.wav");
    fx.levelUp = LoadSound("level_up.wav");
    fx.select = LoadSound("select.wav");

    Player ship = { 0 };
    Bullet* bulletsHead = NULL;
    Asteroid asteroids[MAX_ASTEROIDS] = { 0 };
    Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER] = { 0 };

    Enemy ufos[MAX_UFOS] = { 0 };
    Boss boss = { 0 };

    Particle particles[MAX_PARTICLES] = { 0 };

    HighScoreEntry highScores[MAX_HIGH_SCORES] = { 0 };
    LoadHighScores(highScores);

    int newScoreIndex = -1;
    char nameInput[4] = "AAA";
    int letterIndex = 0;
    int frameCounter = 0;

    float hyperspaceTimer = 0.0f;

    bool isTransitioning = false;

    int score = 0;
    float shootCooldown = 0.0f;
    int level = 1;

    GameScreen currentScreen = SPLASH;
    float splashTimer = 4.5f;

    ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield, particles);
    isTransitioning = false;

    for (int i = 0; i < NUM_LAYERS; i++) {
        for (int j = 0; j < STARS_PER_LAYER; j++) {
            starfield[i][j].x = GetRandomValue(0, screenWidth);
            starfield[i][j].y = GetRandomValue(0, screenHeight);
        }
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        frameCounter++;

        UpdateMusicStream(bgmMenu);
        UpdateMusicStream(bgmGame);

        if (hyperspaceTimer > 0.0f) {
            hyperspaceTimer -= GetFrameTime();
            if (hyperspaceTimer < 0.0f)
                hyperspaceTimer = 0.0f;
        }

        UpdateStarfield(starfield, hyperspaceTimer);

        if (currentScreen == GAMEPLAY || currentScreen == ENDING || currentScreen == NAME_ENTRY || currentScreen == TOP_SCORES) {
            UpdateAsteroids(asteroids);
            UpdateParticles(particles);
        }

        switch (currentScreen) {
        case SPLASH:
            splashTimer -= GetFrameTime();

            if (splashTimer <= 0.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    PlaySound(fx.select);
                }

                ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield, particles);
                isTransitioning = false;
                currentScreen = MENU;
            }
            break;

        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(fx.select);
                ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield, particles);
                isTransitioning = false;
                currentScreen = GAMEPLAY;

                StopMusicStream(bgmMenu);
                PlayMusicStream(bgmGame);
            }
            break;

        case GAMEPLAY:
            UpdatePlayer(&ship);
            UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, false, &fx);
            UpdateBoss(&boss, &ship, &bulletsHead, false, &fx);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, false, &fx, particles);
            CheckLevelClear(asteroids, &level, &ship, &bulletsHead, ufos, &boss, &fx, &hyperspaceTimer, &isTransitioning);

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
                PlaySound(fx.playerDeath);

                SpawnParticles(particles, ship.position, 50, RAYWHITE);

                Bullet* cbToFree = bulletsHead;
                while (cbToFree != NULL) {
                    Bullet* next = cbToFree->next;
                    free(cbToFree);
                    cbToFree = next;
                }
                bulletsHead = NULL;

                if (ship.lives <= 0) {
                    newScoreIndex = -1;
                    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
                        if (score > highScores[i].score) {
                            newScoreIndex = i;
                            break;
                        }
                    }

                    currentScreen = ENDING;

                    for (int e = 0; e < MAX_UFOS; e++) {
                        ufos[e].active = false;
                    }

                    StopMusicStream(bgmGame);
                    PlayMusicStream(bgmMenu);
                } else {
                    ship.position = (Vector2) { screenWidth / 2.0f, screenHeight / 2.0f };
                    ship.velocity = (Vector2) { 0, 0 };
                    ship.invulnerableTimer = 2.0f;
                }
            }
            break;

        case ENDING:
            UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, true, &fx);
            UpdateBoss(&boss, &ship, &bulletsHead, true, &fx);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, true, &fx, particles);

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(fx.select);

                if (newScoreIndex != -1) {
                    currentScreen = NAME_ENTRY;
                    nameInput[0] = 'A';
                    nameInput[1] = 'A';
                    nameInput[2] = 'A';
                    nameInput[3] = '\0';
                    letterIndex = 0;
                } else {
                    currentScreen = TOP_SCORES;
                }
            }
            break;

        case NAME_ENTRY:
            UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, true, &fx);
            UpdateBoss(&boss, &ship, &bulletsHead, true, &fx);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, true, &fx, particles);

            if (IsKeyPressed(KEY_UP)) {
                nameInput[letterIndex]--;
                if (nameInput[letterIndex] < 'A')
                    nameInput[letterIndex] = 'Z';
                PlaySound(fx.select);
            } else if (IsKeyPressed(KEY_DOWN)) {
                nameInput[letterIndex]++;
                if (nameInput[letterIndex] > 'Z')
                    nameInput[letterIndex] = 'A';
                PlaySound(fx.select);
            } else if (IsKeyPressed(KEY_RIGHT)) {
                if (letterIndex < 2) {
                    letterIndex++;
                    PlaySound(fx.select);
                }
            } else if (IsKeyPressed(KEY_LEFT)) {
                if (letterIndex > 0) {
                    letterIndex--;
                    PlaySound(fx.select);
                }
            } else if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(fx.select);
                if (letterIndex < 2) {
                    letterIndex++;
                } else {
                    for (int i = MAX_HIGH_SCORES - 1; i > newScoreIndex; i--) {
                        highScores[i] = highScores[i - 1];
                    }
                    highScores[newScoreIndex].score = score;
                    sprintf(highScores[newScoreIndex].name, "%s", nameInput);

                    SaveHighScores(highScores);
                    currentScreen = TOP_SCORES;
                }
            }
            break;

        case TOP_SCORES:
            UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, true, &fx);
            UpdateBoss(&boss, &ship, &bulletsHead, true, &fx);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, true, &fx, particles);

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(fx.select);
                ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield, particles);
                isTransitioning = false;
                currentScreen = MENU;
            }
            break;
        }

        DrawGame(&ship, bulletsHead, asteroids, ufos, &boss, score, highScores, level, currentScreen, starfield, logoTexture, splashTimer, particles, hyperspaceTimer, nameInput, letterIndex, frameCounter, newScoreIndex);
    }

    UnloadTexture(logoTexture);

    UnloadMusicStream(bgmMenu);
    UnloadMusicStream(bgmGame);
    UnloadSound(fx.shoot);
    UnloadSound(fx.explosion);
    UnloadSound(fx.enemyShoot);
    UnloadSound(fx.enemyExplosion);
    UnloadSound(fx.bossHit);
    UnloadSound(fx.playerDeath);
    UnloadSound(fx.levelUp);
    UnloadSound(fx.select);
    CloseAudioDevice();

    Bullet* currentBullet = bulletsHead;
    while (currentBullet != NULL) {
        Bullet* nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }

    CloseWindow();
    return 0;
}