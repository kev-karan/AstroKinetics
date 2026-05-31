#include "game.h"

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics");

    InitAudioDevice();

    Texture2D logoTexture = LoadTexture("BolaFora-white.png");

    Music bgm = LoadMusicStream("IswearIsawit.ogg");
    PlayMusicStream(bgm);

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

    int score = 0;
    int highScore = LoadHighScore();
    float shootCooldown = 0.0f;
    int level = 1;

    GameScreen currentScreen = SPLASH;
    float splashTimer = 4.5f;

    ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateMusicStream(bgm);

        UpdateStarfield(starfield);

        if (currentScreen == GAMEPLAY || currentScreen == ENDING) {
            UpdateAsteroids(asteroids);
        }

        switch (currentScreen) {
        case SPLASH:
            splashTimer -= GetFrameTime();

            if (splashTimer <= 0.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                PlaySound(fx.select);
                ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);
                currentScreen = MENU;
            }
            break;

        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(fx.select);
                ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);
                currentScreen = GAMEPLAY;
            }
            break;

        case GAMEPLAY:
            UpdatePlayer(&ship);
            UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, false, &fx);
            UpdateBoss(&boss, &ship, &bulletsHead, false, &fx);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, false, &fx);
            CheckLevelClear(asteroids, &level, &ship, &bulletsHead, ufos, &boss, &fx);

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

                Bullet* cbToFree = bulletsHead;
                while (cbToFree != NULL) {
                    Bullet* next = cbToFree->next;
                    free(cbToFree);
                    cbToFree = next;
                }
                bulletsHead = NULL;

                if (ship.lives <= 0) {
                    currentScreen = ENDING;
                    for (int e = 0; e < MAX_UFOS; e++) {
                        ufos[e].active = false;
                    }

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
            UpdateEnemy(ufos, &ship, &bulletsHead, asteroids, true, &fx);
            UpdateBoss(&boss, &ship, &bulletsHead, true, &fx);
            UpdateBullets(&bulletsHead, &ship, &shootCooldown, asteroids, ufos, &boss, &score, true, &fx);

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(fx.select);
                ResetGame(&ship, &bulletsHead, asteroids, ufos, &boss, &score, &level, starfield);
                currentScreen = MENU;
            }
            break;
        }

        DrawGame(&ship, bulletsHead, asteroids, ufos, &boss, score, highScore, level, currentScreen, starfield, logoTexture, splashTimer);
    }

    UnloadTexture(logoTexture);

    UnloadMusicStream(bgm);
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