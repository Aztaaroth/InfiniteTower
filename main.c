#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define GRID_SIZE 5
#define ROOM_SIZE (SCREEN_WIDTH / GRID_SIZE)
#define MAX_ROOMS 10
#define MIN_ROOMS 6
#define PLAYER_SPEED 2.0f

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct Room {
    Vector2 position;
    bool visited;
    int corridorConnections;
    int connectedTo[MAX_ROOMS];
} Room;

Room rooms[MAX_ROOMS];
int totalRooms;
Vector2 playerPos;
Vector2 stairsPos;
bool gamePaused = false;
bool stairsPlaced = false;

float Distance(Vector2 a, Vector2 b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void ConnectRooms(int indexA, int indexB) {
    Vector2 a = (Vector2){rooms[indexA].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[indexA].position.y * ROOM_SIZE + ROOM_SIZE / 2};
    Vector2 b = (Vector2){rooms[indexB].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[indexB].position.y * ROOM_SIZE + ROOM_SIZE / 2};

    if (a.x != b.x) {
        DrawRectangle(MIN(a.x, b.x) - 20, a.y - 20, abs(b.x - a.x) + 40, 40, LIGHTGRAY);
    }
    if (a.y != b.y) {
        DrawRectangle(a.x - 20, MIN(a.y, b.y) - 20, 40, abs(b.y - a.y) + 40, LIGHTGRAY);
    }
}

void GenerateDungeon() {
    totalRooms = GetRandomValue(MIN_ROOMS, MAX_ROOMS);
    playerPos = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    stairsPlaced = false;

    for (int i = 0; i < totalRooms; i++) {
        rooms[i].position = (Vector2){GetRandomValue(0, GRID_SIZE - 1), GetRandomValue(0, GRID_SIZE - 1)};
        rooms[i].visited = false;
        rooms[i].corridorConnections = 0;
        for (int j = 0; j < MAX_ROOMS; j++) {
            rooms[i].connectedTo[j] = -1;
        }
    }

    rooms[0].visited = true;
    playerPos = (Vector2){rooms[0].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[0].position.y * ROOM_SIZE + ROOM_SIZE / 2};

    int distanceRooms = GetRandomValue(4, totalRooms - 1);
    stairsPos = (Vector2){rooms[distanceRooms].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[distanceRooms].position.y * ROOM_SIZE + ROOM_SIZE / 2};
    stairsPlaced = true;
}

void DrawDungeon() {
    for (int i = 0; i < totalRooms; i++) {
        Rectangle room = {rooms[i].position.x * ROOM_SIZE, rooms[i].position.y * ROOM_SIZE, ROOM_SIZE, ROOM_SIZE};
        DrawRectangleRec(room, LIGHTGRAY);
    }
    for (int i = 0; i < totalRooms - 1; i++) {
        ConnectRooms(i, i + 1);
    }
    DrawCircleV(playerPos, 10, BLUE);
    if (stairsPlaced) DrawCircleV(stairsPos, 10, RED);
}

void UpdatePlayer() {
    if (IsKeyDown(KEY_W)) playerPos.y -= PLAYER_SPEED;
    if (IsKeyDown(KEY_S)) playerPos.y += PLAYER_SPEED;
    if (IsKeyDown(KEY_A)) playerPos.x -= PLAYER_SPEED;
    if (IsKeyDown(KEY_D)) playerPos.x += PLAYER_SPEED;

    if (CheckCollisionPointCircle(playerPos, stairsPos, 10)) {
        GenerateDungeon();
    }
}

void PauseMenu() {
    DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, 30, RED);
    DrawText("Press R to Restart", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2, 20, RED);
    DrawText("Press ESC to Quit", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 30, 20, RED);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Infinite Tower");
    SetTargetFPS(60);
    srand(time(NULL));
    GenerateDungeon();

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) gamePaused = !gamePaused;
        if (gamePaused && IsKeyPressed(KEY_R)) {
            GenerateDungeon();
            gamePaused = false;
        }
        if (!gamePaused) {
            UpdatePlayer();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        if (gamePaused) {
            PauseMenu();
        } else {
            DrawDungeon();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
