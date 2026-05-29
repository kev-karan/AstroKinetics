#include "raylib.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "AstroKinetics - BolaFora");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("AstroKinetics TESTE pew pew!", 250, 280, 20, GREEN);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}