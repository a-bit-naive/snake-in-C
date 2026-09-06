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
    RIGHT
} Facing;

typedef struct {
    Vector2 pos;
    Vector2 size;
    int speed;
    Facing direction;
    Vector2 move;
} PLAYER;

void render(PLAYER *p, const GAME_SETTINGS *gs) {
    ClearBackground(BLACK);
    Color background[2] = {GRAY, DARKGRAY};

    BeginDrawing();
    // draw background
        Vector2 location;
        for (int i = 0; i < gs->tileAmount; i++) {
            for (int j = 3; j < gs->tileAmount; j++) {
                location = (Vector2){ i * gs->tileSize.x, j * gs->tileSize.y };
                DrawRectangleV(location, gs->tileSize, background[(i + j) % 2]);
            }
        }
    
       // draw player
        DrawRectangleV(p->pos , p->size, LIME);       

    EndDrawing();
}

void handleInput(PLAYER *p) {
    if (IsKeyPressed(KEY_UP)) p->direction = UP;
    if (IsKeyPressed(KEY_DOWN)) p->direction = DOWN;
    if (IsKeyPressed(KEY_LEFT)) p->direction = LEFT;
    if (IsKeyPressed(KEY_RIGHT)) p->direction = RIGHT;
}
void makeMove(PLAYER *p, const GAME_SETTINGS *gs) {
    switch (p->direction) {
        case UP: 
            p->pos = (Vector2){ p->pos.x, (p->pos.y - gs->tileSize.y) };
            break;
        case DOWN:
            p->pos = (Vector2){ p->pos.x, (p->pos.y + gs->tileSize.y) };
            break;
        case LEFT:
            p->pos = (Vector2){ (p->pos.x - gs->tileSize.x) , p->pos.y };
            break;
       case RIGHT:
            p->pos = (Vector2){ (p->pos.x + gs->tileSize.x) , p->pos.y };
            break;
    }
}


int main(void) {
    // configuration
    GAME_SETTINGS gs = { fps: 60, width: 1024, length: 1024,
        tileSize: { x: 64, y: 64 }, tileAmount: 16 };

    PLAYER p = {
        pos: { 776, 776 },
        size: { 48, 48}, speed: gs.tileSize.x,
        direction: RIGHT };

    SetTargetFPS(gs.fps);
    // configuration end

    InitWindow(gs.width, gs.length, "snake");
    
    int frames;

    while(!WindowShouldClose()) {
        frames++;

        if (frames >= 30) {
            makeMove(&p, &gs);
            frames = 0;
        }
        render(&p, &gs);
        handleInput(&p);
    }

    CloseWindow();
    return 0;
}
