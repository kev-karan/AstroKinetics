#include "raylib.h"
#include <math.h>

typedef struct {
    Vector2 position;
    float rotation;
    float size;
    Color color;
} Player;

const int screenWidth = 800;
const int screenHeight = 600;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AstroKinetics - BolaFora");

    Player ship = { 0 };
    ship.position = (Vector2) { screenWidth / 2.0f, screenHeight / 2.0f };
    ship.rotation = 0.0f;
    ship.size = 20.0f;
    ship.color = RAYWHITE;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        float baseAngle = ship.rotation - 90.0f;

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

        EndDrawing();
    }

    CloseWindow();
    return 0;
}