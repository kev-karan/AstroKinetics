#include "game.h"

void DrawGame(Player* ship, Bullet* bulletsHead, Asteroid* asteroids, Enemy* ufos, Boss* boss, int score, int highScore, int level, GameScreen currentScreen, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER], Texture2D logoTexture, float splashTimer)
{
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < NUM_LAYERS; i++) {
        Color starColor;
        if (i == 0)
            starColor = DARKGRAY;
        else if (i == 1)
            starColor = GRAY;
        else
            starColor = LIGHTGRAY;

        for (int j = 0; j < STARS_PER_LAYER; j++) {
            DrawPixelV(starfield[i][j], starColor);
        }
    }

    if (currentScreen == GAMEPLAY || currentScreen == ENDING) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                for (int j = 0; j < ASTEROID_VERTICES; j++) {
                    float angle1 = (j * 360.0f / ASTEROID_VERTICES) * DEG2RAD;
                    float angle2 = (((j + 1) % ASTEROID_VERTICES) * 360.0f / ASTEROID_VERTICES) * DEG2RAD;

                    float r1 = asteroids[i].radius * asteroids[i].vertexOffsets[j];
                    float r2 = asteroids[i].radius * asteroids[i].vertexOffsets[(j + 1) % ASTEROID_VERTICES];

                    Vector2 p1 = { asteroids[i].position.x + cosf(angle1) * r1, asteroids[i].position.y + sinf(angle1) * r1 };
                    Vector2 p2 = { asteroids[i].position.x + cosf(angle2) * r2, asteroids[i].position.y + sinf(angle2) * r2 };

                    DrawLineV(p1, p2, RAYWHITE);
                }
            }
        }

        for (int e = 0; e < MAX_UFOS; e++) {
            if (ufos[e].active) {
                DrawEllipseLines(ufos[e].position.x, ufos[e].position.y, ufos[e].radius, ufos[e].radius * 0.4f, RED);
                DrawEllipseLines(ufos[e].position.x, ufos[e].position.y - 4, ufos[e].radius * 0.5f, ufos[e].radius * 0.4f, RED);
            }
        }

        if (boss->active) {
            DrawPoly(boss->position, 8, boss->radius, 0, PURPLE);
            DrawPolyLines(boss->position, 8, boss->radius, 0, MAGENTA);

            float hpPercentage = (float)boss->health / boss->maxHealth;
            DrawRectangle(boss->position.x - 40, boss->position.y - boss->radius - 20, 80 * hpPercentage, 8, RED);
            DrawRectangleLines(boss->position.x - 40, boss->position.y - boss->radius - 20, 80, 8, WHITE);
        }

        Bullet* currentBullet = bulletsHead;
        while (currentBullet != NULL) {
            Color bulletColor = currentBullet->isEnemy ? RED : WHITE;
            DrawCircleV(currentBullet->position, 2.0f, bulletColor);
            currentBullet = currentBullet->next;
        }
    }

    switch (currentScreen) {
    case SPLASH: {
        float scale = 0.18f;

        Vector2 logoPos = {
            (screenWidth / 2.0f) - ((logoTexture.width * scale) / 2.0f),
            (screenHeight / 2.0f) - ((logoTexture.height * scale) / 2.0f)
        };

        float alpha = 1.0f;

        if (splashTimer > 3.0f) {
            alpha = 4.0f - splashTimer;
        } else if (splashTimer < 1.5f) {
            alpha = splashTimer;
        }

        if (alpha < 0.0f)
            alpha = 0.0f;
        if (alpha > 1.0f)
            alpha = 1.0f;

        DrawTextureEx(logoTexture, logoPos, 0.0f, scale, Fade(WHITE, alpha));
    } break;

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

        Vector2 p1 = { ship->position.x + cosf(baseAngle * DEG2RAD) * ship->size,
            ship->position.y + sinf(baseAngle * DEG2RAD) * ship->size };

        Vector2 p2 = { ship->position.x + cosf((baseAngle + 140.0f) * DEG2RAD) * ship->size,
            ship->position.y + sinf((baseAngle + 140.0f) * DEG2RAD) * ship->size };

        Vector2 p3 = { ship->position.x + cosf((baseAngle + 180.0f) * DEG2RAD) * (ship->size * 0.4f),
            ship->position.y + sinf((baseAngle + 180.0f) * DEG2RAD) * (ship->size * 0.4f) };

        Vector2 p4 = { ship->position.x + cosf((baseAngle - 140.0f) * DEG2RAD) * ship->size,
            ship->position.y + sinf((baseAngle - 140.0f) * DEG2RAD) * ship->size };

        bool drawShip = true;
        if (ship->invulnerableTimer > 0.0f) {
            if ((int)(ship->invulnerableTimer * 10) % 2 == 0) {
                drawShip = false;
            }
        }

        if (drawShip) {
            DrawLineEx(p1, p2, 2.0f, ship->color);
            DrawLineEx(p2, p3, 2.0f, ship->color);
            DrawLineEx(p3, p4, 2.0f, ship->color);
            DrawLineEx(p4, p1, 2.0f, ship->color);

            if (IsKeyDown(KEY_UP)) {
                float flicker = GetRandomValue(8, 14) / 10.0f;

                Vector2 flameTip = { ship->position.x + cosf((baseAngle + 180.0f) * DEG2RAD) * (ship->size * flicker),
                    ship->position.y + sinf((baseAngle + 180.0f) * DEG2RAD) * (ship->size * flicker) };

                Vector2 flameBaseR = { ship->position.x + cosf((baseAngle + 155.0f) * DEG2RAD) * (ship->size * 0.6f),
                    ship->position.y + sinf((baseAngle + 155.0f) * DEG2RAD) * (ship->size * 0.6f) };

                Vector2 flameBaseL = { ship->position.x + cosf((baseAngle - 155.0f) * DEG2RAD) * (ship->size * 0.6f),
                    ship->position.y + sinf((baseAngle - 155.0f) * DEG2RAD) * (ship->size * 0.6f) };

                DrawLineEx(flameBaseR, flameTip, 2.0f, ORANGE);
                DrawLineEx(flameBaseL, flameTip, 2.0f, ORANGE);
            }
        }

        DrawText(TextFormat("SCORE: %05i", score), 20, 20, 30, RAYWHITE);
        DrawText(TextFormat("HI-SCORE: %05i", highScore), 20, 60, 20, GRAY);
        DrawText(TextFormat("LIVES: %i", ship->lives), 20, 90, 20, RAYWHITE);
        DrawText(TextFormat("WAVE %i", level), screenWidth / 2 - 40, 20, 20, RAYWHITE);

        if (boss->introTimer > 0.0f) {
            int warningWidth = MeasureText("WARNING: BOSS BATTLE", 40);
            DrawText("WARNING: BOSS BATTLE", screenWidth / 2 - warningWidth / 2, screenHeight / 2 - 60, 40, RED);

            int secondsLeft = (int)ceilf(boss->introTimer);
            int countWidth = MeasureText(TextFormat("%i", secondsLeft), 50);
            DrawText(TextFormat("%i", secondsLeft), screenWidth / 2 - countWidth / 2, screenHeight / 2 + 10, 50, RAYWHITE);
        }
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