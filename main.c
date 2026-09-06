#include <stdlib.h>
#include <time.h>

#include "raylib.h"

typedef struct {
    int fps;
    int width;
    int length;
    Vector2 tileSize;
    int tileAmount;
    int score;
    int highScore;
} GAME_DATA;

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

typedef struct {
    Vector2 pos;
    Vector2 size;
    Color color;
} APPLE;

void render(PLAYER *p, const GAME_DATA *gd, APPLE *a) {
    ClearBackground(BLACK);
    Color background[2] = {GRAY, DARKGRAY};

    BeginDrawing();
    // draw background
        Vector2 location;
        for (int i = 0; i < gd->tileAmount; i++) {
            for (int j = 3; j < gd->tileAmount; j++) {
                location = (Vector2){ i * gd->tileSize.x, j * gd->tileSize.y };
                DrawRectangleV(location, gd->tileSize, background[(i + j) % 2]);
            }
        }
 
        // draw apple
        DrawRectangleV(a->pos, a->size, a->color);
   
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
void makeMove(PLAYER *p, const GAME_DATA *gd) {
    float top = 3 * gd->tileSize.y;
    float bottom = (gd->tileAmount) * gd->tileSize.y;
    float left = 0;
    float right = (gd->tileAmount) * gd->tileSize.x;

    int playerTileOffset = (gd->tileSize.x - p->size.x) / 2;

    switch (p->direction) {
        case UP: 
            p->pos.y -= gd->tileSize.y;
            if (p->pos.y < top) p->pos.y = bottom - gd->tileSize.y + playerTileOffset;
            break;
        case DOWN:
            p->pos.y += gd->tileSize.y;
            if (p->pos.y > bottom) p->pos.y = top + playerTileOffset;
            break;
        case LEFT:
            p->pos.x -= gd->tileSize.x;
            if (p->pos.x < left) p->pos.x = right - gd->tileSize.x + playerTileOffset;
            break;
       case RIGHT:
            p->pos.x += gd->tileSize.x;
            if (p->pos.x > right) p->pos.x = left + playerTileOffset;
            break;
    }
}

bool checkApple(APPLE *a, const PLAYER *p, const GAME_DATA *gd) {
    Rectangle appleRect = {
        a->pos.x,
        a->pos.y,
        a->size.x,
        a->size.y
    };

    Rectangle playerRect = {
        p->pos.x,
        p->pos.y,
        p->size.x,
        p->size.y
    };

    return CheckCollisionRecs(appleRect, playerRect);
}

void generateNewApple(APPLE *a, PLAYER *p, GAME_DATA *gd) {
    APPLE tmp;

    do {
        tmp = (APPLE){
            .pos = (Vector2){ (rand() % gd->tileAmount) * (float)gd->tileSize.x
                                + (gd->tileSize.x - a->size.x) / 2,
                     (rand() % gd->tileAmount) * (float)gd->tileSize.y
                                + (gd->tileSize.y - a->size.y) / 2 },
            .size = (Vector2){ 42, 42 },
            .color = RED
        };
    } while ((tmp.pos.y < 3 * gd->tileSize.y) || checkApple(&tmp, p, gd));

    a->pos = tmp.pos;
}

int main(void) { srand(time(NULL));
    // configuration
    GAME_DATA gd = { .fps = 60, .width = 1024, .length = 1024,
        .tileSize =  { .x = 64, .y = 64 }, .tileAmount = 16,
        .score = 0, .highScore = 20 };

    PLAYER p = {
        .pos = { 584, 584 },
        .size = { 48, 48}, .speed = gd.tileSize.x,
        .direction = RIGHT };

    APPLE a = {
        .pos = { 779, 779 },
        .size = { 42, 42 },
        .color = RED
    };

    SetTargetFPS(gd.fps);
    // configuration end

    InitWindow(gd.width, gd.length, "snake");
    
    int frames = 0;

    while(!WindowShouldClose()) {
        frames++;

        if (frames >= 30) {
            makeMove(&p, &gd);
            if (checkApple(&a, &p, &gd)) {
                gd.score++;
                generateNewApple(&a, &p, &gd);
            }
            frames = 0;
        }
        render(&p, &gd, &a);
        handleInput(&p);
    }

    CloseWindow();
    return 0;
}
