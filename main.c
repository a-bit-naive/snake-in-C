#include <stdio.h>
#include "raylib.h"

typedef struct {
    int fps;
    int width;
    int length;
} GAME_SETTINGS;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
} Dir;

typedef struct {
    Vector2 pos;
    Vector2 size;
    int speed;
    Dir direction;
    Vector2 move;
} PLAYER;

void resetMovement(PLAYER *p) {
    p->move.x = 0;
    p->move.y = 0;
}

void render(PLAYER *p) {
    BeginDrawing();
        ClearBackground(BLACK);
        // draw player
        DrawRectangleV( p->pos , p->size, GREEN);       
    EndDrawing();
}

void handleInput(PLAYER *p) {
    if (IsKeyPressed(KEY_UP) && p->direction != UP) {
        resetMovement(p);
        p->move.y -= p->speed;
        p->direction = UP;
    }
    if (IsKeyPressed(KEY_DOWN) && p->direction != DOWN) {
        resetMovement(p);
        p->move.y += p->speed;
        p->direction = DOWN;
    }
    if (IsKeyPressed(KEY_LEFT) && p->direction != LEFT) {
        resetMovement(p);
        p->move.x -= p->speed;
        p->direction = LEFT;
    }
    if (IsKeyPressed(KEY_RIGHT) && p->direction != RIGHT) {
        resetMovement(p);
        p->move.x += p->speed;
        p->direction = RIGHT;
    }
    
    p->pos.x += p->move.x;
    p->pos.y += p->move.y;
}


int main(void) {

    GAME_SETTINGS gs = { fps: 60, width: 800, length: 400 };
    PLAYER p = { pos: { 400, 200}, size: { 32, 32}, speed: 2, direction: NONE};


    SetTargetFPS(gs.fps);

    InitWindow(gs.width, gs.length, "snake");
    
    while(!WindowShouldClose()) {
        render(&p);
        handleInput(&p);
    }  

    CloseWindow();
    return 0;
}
