#include "game.h"

void DrawGame(Player* ship, Bullet* bulletsHead, Asteroid* asteroids, Enemy* ufo, int score, int highScore, int level, GameScreen currentScreen, Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER])
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

    if (ufo->active) {
        DrawEllipseLines(ufo->position.x, ufo->position.y, ufo->radius, ufo->radius * 0.4f, RED);
        DrawEllipseLines(ufo->position.x, ufo->position.y - 4, ufo->radius * 0.5f, ufo->radius * 0.4f, RED);
    }

    if (currentScreen != MENU) {
        Bullet* currentBullet = bulletsHead;
        while (currentBullet != NULL) {
            Color bulletColor = currentBullet->isEnemy ? RED : WHITE;
            DrawCircleV(currentBullet->position, 2.0f, bulletColor);
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

        DrawText(TextFormat("WAVE %i", level), screenWidth / 2 - 40, 20, 20, RAYWHITE);
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