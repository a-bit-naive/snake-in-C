#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "stack.h"

typedef enum {
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_START_GAME,
    ACTION_START_GAME2,
    ACTION_START_HYPER,
    ACTION_GOTO_MENU,
    ACTION_RESTART_GAME,
    ACTION_SETTINGS_MENU,
} ActionType;

typedef struct {
    int key;
} Action;

static Action bindings[] = {
    [ACTION_MOVE_UP] =  KEY_A,
    [ACTION_MOVE_DOWN] = KEY_S,
    [ACTION_MOVE_LEFT] = KEY_A,
    [ACTION_MOVE_RIGHT] = KEY_D,
    [ACTION_START_GAME] = KEY_SPACE,
    [ACTION_START_GAME2] = KEY_ENTER,
    [ACTION_GOTO_MENU] = KEY_Q,
    [ACTION_RESTART_GAME] = KEY_R,
    [ACTION_SETTINGS_MENU] = KEY_O,
};

typedef enum {
    TITLE,
    GAME,
    OVER,
    RESTART
} State;

typedef enum {
    NORMAL,
    HYPER
} Mode;

typedef struct {
    int fps;
    int width;
    int length;
    Vector2 tileSize;
    int tileAmount;
    int score;
    int highScore;
    int initHighScore;
    State state;
    Font font;
    Mode mode;
} GAME_DATA;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Facing;

typedef struct {
    Vector2 pos;
    Vector2 prevPos; 
    Vector2 size;
    float speed;
    Facing direction;
    Vector2 move;
    Stack snake;
} PLAYER;

typedef struct {
    Vector2 pos;
    Vector2 size;
    Color color;
} APPLE;

void BindKey(ActionType type, int key) {
    bindings[type].key = key;
}

bool IsActionPressed(ActionType type) {
    return IsKeyPressed(bindings[type].key);
}

void render(PLAYER *p, const GAME_DATA *gd, APPLE *a) {
    ClearBackground(BLACK);
    Color background[2] = {GRAY, DARKGRAY};

        Vector2 location;
        for (int i = 0; i < gd->tileAmount; i++) {
            for (int j = 2; j < gd->tileAmount - 1; j++) {
                location = (Vector2){ i * gd->tileSize.x, j * gd->tileSize.y };
                DrawRectangleV(location, gd->tileSize, background[(i + j) % 2]);
            }
        }
        // draw UI
        DrawText(TextFormat("Score: %d", gd->score), gd->tileSize.x * (gd->tileAmount - 3), 1.5 * gd->tileSize.y, 30, WHITE);
        DrawText(TextFormat("Best Score: %d", gd->highScore), gd->tileSize.x * 1, 1.5 * gd->tileSize.y, 30, YELLOW);

        if (gd->mode == HYPER) {
            char *maxSpeed = p->speed >= 10.0f ? "(MAX)" : "";
            DrawText(TextFormat("Current Speed: %.2f %s", p->speed, maxSpeed), gd->tileSize.x * ((float)gd->tileAmount / 2 + 1.85), (gd->tileAmount - 0.5) * gd->tileSize.y, 30, PURPLE);
        }
        // draw apple
        DrawRectangleV(a->pos, a->size, a->color);
   
        // draw snake  head
        DrawRectangleV(p->pos , p->size, GREEN);       

        // draw snake body
        for (int i = 0; i < p->snake.size; i++) {
            Vector2 *coord = p->snake.data[i];
            DrawRectangleV(*coord, p->size, LIME);
        }

}

void handleInput(PLAYER *p) {
    if (IsActionPressed(ACTION_MOVE_UP) && p->direction != DOWN)    p->direction = UP;
    if (IsActionPressed(ACTION_MOVE_DOWN) && p->direction != UP)    p->direction = DOWN;
    if (IsActionPressed(ACTION_MOVE_LEFT) && p->direction != RIGHT) p->direction = LEFT;
    if (IsActionPressed(ACTION_MOVE_RIGHT) && p->direction != LEFT) p->direction = RIGHT;
}
void makeMove(PLAYER *p, const GAME_DATA *gd) {
    p->prevPos = p->pos;

    float top = 2 * gd->tileSize.y;
    float bottom = (gd->tileAmount - 1) * gd->tileSize.y;
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

void moveBody(PLAYER *p) {
    for (int i = p->snake.size - 1; i > 0; i--) {
        Vector2 *curr = p->snake.data[i];
        Vector2 *prev = p->snake.data[i - 1];

        *curr = *prev;
    }
    if (p->snake.size <= 0) return;
    Vector2 *first = p->snake.data[0];
    *first = p->pos;
}

bool checkApple(APPLE *a, const PLAYER *p, GAME_DATA *gd) {
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

    if (gd->score > gd->highScore) {
        gd->highScore = gd->score;
    }

    return CheckCollisionRecs(appleRect, playerRect);
}

void generateNewApple(APPLE *a, PLAYER *p, GAME_DATA *gd) {
    APPLE tmp;

    do {
        tmp = (APPLE){
            .pos = (Vector2){ (rand() % gd->tileAmount) * (float)gd->tileSize.x
                                + (gd->tileSize.x - a->size.x) / 2,
                     (rand() % (gd->tileAmount - 3) + 2) * (float)gd->tileSize.y
                                + (gd->tileSize.y - a->size.y) / 2 },
            .size = (Vector2){ 42, 42 },
            .color = RED
        };
    } while (checkApple(&tmp, p, gd));

    a->pos = tmp.pos;
}

void growSnake(PLAYER *p) {
    Vector2 *newPart = malloc(sizeof(*newPart));
    if (newPart == NULL) return;

    if (p->snake.size == 0) *newPart = p->prevPos;
    else
        *newPart = *(Vector2 *)p->snake.data[p->snake.size - 1];

    push(&p->snake, newPart);
}

bool checkCollisions(PLAYER *p) {
    Rectangle head = (Rectangle){ p->pos.x, p->pos.y, p->size.x, p->size.y };

    for (int i = 0; i < p->snake.size; i++) {
        Vector2 *tmp = p->snake.data[i];

        Rectangle bodyPart = (Rectangle){ tmp->x, tmp->y, p->size.x, p->size.y };

        if (CheckCollisionRecs(head, bodyPart)) return true;
    }

    return false;
}

void renderTitleScreen(const GAME_DATA *gd, Font font) {
    float middleofscreen = (gd->tileSize.x * gd->tileAmount / 2);
    const char *titlescreen[] = {
        "Snake in C",
        "by naively",
        "Press [ENTER] or [SPACE] to start",
        "Press [H] for HYPERMODE"
    };
    float h1Size = 60.0f;
    float h2Size = 25.0f;
    float h3Size = 20.0f;

    float spacing1 = 5.0f;
    float spacing2 = 4.0f;
    float spacing3 = 5.0f;

    Vector2 size1 = MeasureTextEx(font, titlescreen[0], h1Size, spacing1);
    Vector2 size2 = MeasureTextEx(font, titlescreen[1], h3Size, spacing3);
    Vector2 size3 = MeasureTextEx(font, titlescreen[2], h2Size, spacing2);
    Vector2 size4 = MeasureTextEx(font, titlescreen[3], h2Size, spacing2);

        ClearBackground(BLACK);

        DrawTextEx(font, titlescreen[0],  (Vector2){ middleofscreen -  size1.x / 2.0f, middleofscreen * 0.78f }, h1Size, spacing1, LIME);
        DrawTextEx(font, titlescreen[1], (Vector2){ middleofscreen -  size2.x / 2.0f, middleofscreen *  0.95f }, h3Size, spacing3, PURPLE);
        DrawTextEx(font, titlescreen[2], (Vector2){ middleofscreen -  size3.x / 2.0f, middleofscreen *  1.3f }, h2Size, spacing2, WHITE);
        DrawTextEx(font, titlescreen[3], (Vector2){ middleofscreen -  size4.x / 2.0f, middleofscreen *  1.5f }, h2Size, spacing2, GRAY);

}

void handleTitleScreenInputs(GAME_DATA *gd) {
    if (IsActionPressed(ACTION_START_GAME)) {
        gd->state = GAME;    
    }
    if (IsActionPressed(ACTION_START_HYPER)) {
        gd->state = GAME;
        gd->mode = HYPER;
    }
}

void renderGameOverScreen(const GAME_DATA *gd, Font font) {
    const char *newHighScore = gd->score > gd->initHighScore ? "NEW HIGHSCORE!" : "";

    float middleofscreen = (gd->tileSize.x * gd->tileAmount / 2);
    const char *gameOverScreen[] = {
        "Game Over",
        TextFormat("Score: %d", gd->score),
        TextFormat("%s", newHighScore),
        "   Press [R] to restart\n\nPress [Q] to quit to titlescreen"
    };

    float h1Size = 60.0f;
    float h2Size = 25.0f;
    float h3Size = 20.0f;

    float spacing1 = 5.0f;
    float spacing2 = 4.0f;
    float spacing3 = 2.0f;

    Vector2 size1 = MeasureTextEx(font, gameOverScreen[0], h1Size, spacing1);
    Vector2 size2 = MeasureTextEx(font, gameOverScreen[1], h2Size, spacing2);
    Vector2 size3 = MeasureTextEx(font, gameOverScreen[2], h2Size, spacing2);
    Vector2 size4 = MeasureTextEx(font, gameOverScreen[3], h3Size, spacing3);

        ClearBackground(BLACK);
        DrawTextEx(font, gameOverScreen[0], (Vector2){ middleofscreen - size1.x / 2.0f, middleofscreen * 0.65 }, h1Size, spacing1, RED);
        DrawTextEx(font, gameOverScreen[1], (Vector2){ middleofscreen - size2.x / 2.0f, middleofscreen * 1 }, h2Size, spacing2, GREEN);
        DrawTextEx(font, gameOverScreen[2], (Vector2){ middleofscreen - size3.x / 2.0f, middleofscreen * 1.1 }, h2Size, spacing2, PURPLE);
        DrawTextEx(font, gameOverScreen[3], (Vector2){ middleofscreen - size4.x / 2.0f, middleofscreen * 1.55 }, h3Size, spacing3, WHITE);
}

void handleGameOverInputs(GAME_DATA *gd, bool *gameoverInit) {
    if (IsActionPressed(ACTION_RESTART_GAME)) {
        gd->state = RESTART;
        gameoverInit = false;
    }
    if (IsActionPressed(ACTION_GOTO_MENU)) {
        gd->state = TITLE;
        gameoverInit = false;
    }
}

GAME_DATA initGameData(void) {
    char *config = LoadFileText("./data/config.txt");
    
    int highScore = 0;

    if (config != NULL) {
        sscanf(config, "highscore: %d", &highScore);
        UnloadFileText(config);
    }

    return (GAME_DATA) {
        .fps = 60, .width = 1024, .length = 1024,
        .tileSize =  { .x = 64, .y = 64 }, .tileAmount = 16,
        .score = 0, .highScore = highScore, .initHighScore = highScore, 
        .state = TITLE, .mode = NORMAL
    };
}

PLAYER initPlayer(const GAME_DATA *gd) {
    return (PLAYER) {
        .pos = { 584, 584 },
        .size = { 48, 48}, .speed = 2.5f,
        .direction = RIGHT
    };
}

APPLE initApple(void) {
    return (APPLE) {
        .pos = { 779, 779 },
        .size = { 42, 42 },
        .color = RED
    };
}

Rectangle GetGameDestination(int gameWidth, int gameHeight) {
    float screenWidth  = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();

    float scaleX = screenWidth / gameWidth;
    float scaleY = screenHeight / gameHeight;

    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    float width  = gameWidth * scale;
    float height = gameHeight * scale;

    return (Rectangle) {
        (screenWidth  - width)  / 2.0f,
        (screenHeight - height) / 2.0f,
        width,
        height
    };
}

int main(void) {
     // Init Setup
    srand(time(NULL));

    GAME_DATA gd = initGameData();
    PLAYER p = initPlayer(&gd);
    APPLE a = initApple();

    SetTargetFPS(gd.fps);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(gd.width, gd.length, "snake");
    
    RenderTexture2D target = LoadRenderTexture(gd.width, gd.length);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    // Init Setup end
    
    Font font = LoadFontEx("./assets/fonts/ARCADE_N.TTF", 32, 0, 250);
    int frames = 0;

    bool gameOverInit = false;
    float MAX_SPEED = 10.0f;
    float moveTimer = 0.0f;

    while(!WindowShouldClose()) {

        if (gd.state == GAME) {
            float dt = GetFrameTime();
            float moveInterval = 1.0f / p.speed;

            handleInput(&p);

            moveTimer += dt;

            if (moveTimer > moveInterval) {
                moveBody(&p);
                makeMove(&p, &gd);
                if (checkCollisions(&p)) {
                    gd.state = OVER;
                }

                if (checkApple(&a, &p, &gd)) {
                    gd.score++;
                    growSnake(&p);
                    generateNewApple(&a, &p, &gd);
                    if (gd.mode == HYPER) {
                        if (p.speed < MAX_SPEED)
                            p.speed += 0.25f;
                    }
                }
                moveTimer -= moveInterval;
            }
        }
        if (gd.state == TITLE) {
            handleTitleScreenInputs(&gd);
        }

        if (gd.state == OVER) {
            if (!gameOverInit) {
                char *config = LoadFileText("./data/config.txt");
                if (config != NULL) {
                    char buffer[64];
                    sprintf(buffer, "highscore: %d", gd.highScore);
                    SaveFileText("./data/config.txt", buffer);
                }
                UnloadFileText(config);
                gameOverInit = true;
            }
            

            handleGameOverInputs(&gd, &gameOverInit);
        }

        if (gd.state == RESTART) {
            gd = initGameData();
            p = initPlayer(&gd);
            a = initApple();
            gd.state = GAME;
        }

        BeginTextureMode(target);
            ClearBackground(BLACK);

            switch (gd.state) {
                case TITLE: renderTitleScreen(&gd, font); break;
                case GAME: render(&p, &gd, &a); break;
                case OVER: renderGameOverScreen(&gd, font); break;
                default: break;
            }
        EndTextureMode();
        
        Rectangle destination =
        GetGameDestination(gd.width, gd.length);

        Rectangle source = {
            0,
            0,
            (float)target.texture.width,
            -(float)target.texture.height
        };

        BeginDrawing();

            ClearBackground(BLACK);

            DrawTexturePro(
                target.texture,
                source,
                destination,
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

        EndDrawing();
    }

    UnloadFont(font);
    destroyStack(&p.snake);
    CloseWindow();
    return 0;
}
