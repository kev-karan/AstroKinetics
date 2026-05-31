#include "game.h"

void DrawGame(Player* ship, Bullet* bulletsHead, Asteroid* asteroids, Enemy* ufos, Boss* boss, int score, HighScoreEntry* highScores, int level, GameScreen currentScreen, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER], Texture2D logoTexture, float splashTimer, Particle* particles, float hyperspaceTimer, char* nameInput, int letterIndex, int frameCounter, int newScoreIndex)
{
    BeginDrawing();
    ClearBackground(BLACK);

    float warpFactor = 0.0f;
    if (hyperspaceTimer > 0.0f) {
        warpFactor = sinf((hyperspaceTimer / 3.0f) * PI);
    }

    for (int i = 0; i < NUM_LAYERS; i++) {
        Color starColor;
        if (i == 0)
            starColor = DARKGRAY;
        else if (i == 1)
            starColor = GRAY;
        else
            starColor = LIGHTGRAY;

        for (int j = 0; j < STARS_PER_LAYER; j++) {
            if (warpFactor > 0.01f) {
                float length = (i + 1) * 20.0f * warpFactor;
                DrawLineV(starfield[i][j], (Vector2) { starfield[i][j].x, starfield[i][j].y - length }, starColor);
            } else {
                DrawPixelV(starfield[i][j], starColor);
            }
        }
    }

    if (currentScreen == GAMEPLAY || currentScreen == ENDING || currentScreen == NAME_ENTRY || currentScreen == TOP_SCORES) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                for (int j = 0; j < ASTEROID_VERTICES; j++) {
                    float angle1 = (asteroids[i].rotation + j * 360.0f / ASTEROID_VERTICES) * DEG2RAD;
                    float angle2 = (asteroids[i].rotation + ((j + 1) % ASTEROID_VERTICES) * 360.0f / ASTEROID_VERTICES) * DEG2RAD;

                    float r1 = asteroids[i].radius * asteroids[i].vertexOffsets[j];
                    float r2 = asteroids[i].radius * asteroids[i].vertexOffsets[(j + 1) % ASTEROID_VERTICES];

                    Vector2 p1 = { asteroids[i].position.x + cosf(angle1) * r1, asteroids[i].position.y + sinf(angle1) * r1 };
                    Vector2 p2 = { asteroids[i].position.x + cosf(angle2) * r2, asteroids[i].position.y + sinf(angle2) * r2 };

                    DrawLineV(p1, p2, RAYWHITE);
                }
            }
        }

        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].active) {
                float alpha = particles[i].lifeTime;
                if (alpha > 1.0f)
                    alpha = 1.0f;
                if (alpha < 0.0f)
                    alpha = 0.0f;

                DrawCircleV(particles[i].position, 1.5f, Fade(particles[i].color, alpha));
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

        const char* hsText = TextFormat("HI-SCORE: %s %05i", highScores[0].name, highScores[0].score);
        int hsWidth = MeasureText(hsText, 20);
        DrawText(hsText, (screenWidth / 2) - (hsWidth / 2), screenHeight / 2 + 80, 20, DARKGRAY);
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
        DrawText(TextFormat("HI-SCORE: %s %05i", highScores[0].name, highScores[0].score), 20, 60, 20, GRAY);

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

        int gameOverWidth = MeasureText("GAME OVER", 60);
        DrawText("GAME OVER", (screenWidth / 2) - (gameOverWidth / 2), screenHeight / 2 - 50, 60, RED);

        if (newScoreIndex != -1) {
            int recordWidth = MeasureText("NEW RECORD!", 30);
            DrawText("NEW RECORD!", (screenWidth / 2) - (recordWidth / 2), screenHeight / 2 + 20, 30, YELLOW);

            int restartWidth = MeasureText("PRESS ENTER TO REGISTER", 20);
            DrawText("PRESS ENTER TO REGISTER", (screenWidth / 2) - (restartWidth / 2), screenHeight / 2 + 70, 20, GRAY);
        } else {
            int restartWidth = MeasureText("PRESS ENTER TO CONTINUE", 20);
            DrawText("PRESS ENTER TO CONTINUE", (screenWidth / 2) - (restartWidth / 2), screenHeight / 2 + 30, 20, GRAY);
        }
    } break;

    case NAME_ENTRY: {
        DrawText(TextFormat("SCORE: %05i", score), 20, 20, 30, RAYWHITE);

        int subMsgWidth = MeasureText("ENTER YOUR INITIALS", 20);
        DrawText("ENTER YOUR INITIALS", (screenWidth / 2) - (subMsgWidth / 2), screenHeight / 2 - 40, 20, GRAY);

        for (int i = 0; i < 3; i++) {
            Color letterColor = (i == letterIndex) ? RAYWHITE : DARKGRAY;
            DrawText(TextFormat("%c", nameInput[i]), (screenWidth / 2) - 40 + (i * 30), screenHeight / 2 + 10, 40, letterColor);

            if (i == letterIndex && (frameCounter / 15) % 2 == 0) {
                DrawText("_", (screenWidth / 2) - 40 + (i * 30), screenHeight / 2 + 15, 40, GRAY);
            }
        }

        int hintWidth = MeasureText("ARROWS to change - ENTER to confirm", 20);
        DrawText("ARROWS to change - ENTER to confirm", (screenWidth / 2) - (hintWidth / 2), screenHeight / 2 + 100, 20, DARKGRAY);

    } break;

    case TOP_SCORES: {
        int topTitleWidth = MeasureText("TOP 5 PILOTS", 30);
        DrawText("TOP 5 PILOTS", (screenWidth / 2) - (topTitleWidth / 2), 150, 30, GRAY);

        for (int i = 0; i < MAX_HIGH_SCORES; i++) {
            Color rowColor = (i == 0) ? YELLOW : LIGHTGRAY;
            const char* rank = TextFormat("%d.", i + 1);
            const char* name = highScores[i].name;
            const char* pts = TextFormat("%05i", highScores[i].score);

            DrawText(rank, (screenWidth / 2) - 120, 210 + (i * 35), 20, rowColor);
            DrawText(name, (screenWidth / 2) - 60, 210 + (i * 35), 20, rowColor);
            DrawText(pts, (screenWidth / 2) + 40, 210 + (i * 35), 20, rowColor);
        }

        int restartWidth = MeasureText("PRESS ENTER TO RETURN TO MENU", 20);
        DrawText("PRESS ENTER TO RETURN TO MENU", (screenWidth / 2) - (restartWidth / 2), screenHeight - 80, 20, GRAY);
    } break;
    }

    EndDrawing();
}