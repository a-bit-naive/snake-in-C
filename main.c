#include <stdio.h>
#include "raylib.h"

typedef struct {
    int fps;
    int width;
    int length;
} GAME_SETTINGS;

typedef struct {
    Vector2 pos;
    int speed;
} PLAYER;

void render(void) {
    BeginDrawing();
    
    

    EndDrawing();
}

void handleInput(void) {
    if (IsKeyPressed(KEY_UP)) printf("Up");
    if (IsKeyPressed(KEY_DOWN)) printf("Down");
    if (IsKeyPressed(KEY_LEFT)) printf("Left");
    if (IsKeyPressed(KEY_RIGHT)) printf("Right");
}


int main(void) {

    GAME_SETTINGS gs = { fps: 60, width: 800, length: 400 };
    PLAYER p = { { x: 100, y: 100}, 10 };


    
    SetTargetFPS(gs.fps);

    InitWindow(gs.width, gs.length, "snake");
    
    while(!WindowShouldClose()) {
        render();
        handleInput();
    }  

    CloseWindow();
    return 0;
}
