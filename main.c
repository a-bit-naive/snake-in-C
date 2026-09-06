#include <stdio.h>
#include "raylib.h"

typedef struct {
    int fps;
    int width;
    int length;
    Vector2 tileSize;
    int tileAmount;
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
       // draw player
        DrawRectangleV(p->pos , p->size, GREEN);       
    EndDrawing();

}

// TODO: use vectormath (module: raymath)
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
    // configuration
    GAME_SETTINGS gs = { fps: 60, width: 1600, length: 1024,
        tileSize: { x: 32, y: 32 }, tileAmount: 50 };

    PLAYER p = { pos: { 400, 200}, size: { 32, 32}, speed: 2,
        direction: NONE };

    SetTargetFPS(gs.fps);
    // configuration end

    InitWindow(gs.width, gs.length, "snake");
    
    // draw background grid
    Color background[2] = {GRAY, BLACK};
    BeginDrawing();
        Vector2 location;
        int index;
        for (int i = 0; i < gs.tileAmount; i++) {
            for (int j = 4; j < gs.tileAmount; j++) {
                location = (Vector2){ i * gs.tileSize.x, j * gs.tileSize.x };
                DrawRectangleV(location, gs.tileSize, background[index % 2]);
                index++;
            }
            index++;
        }
    EndDrawing();

    while(!WindowShouldClose()) {
        render(&p);
        handleInput(&p);
    }  

    CloseWindow();
    return 0;
}
