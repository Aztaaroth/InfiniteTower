#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define GRID_SIZE 5
#define ROOM_SIZE (SCREEN_WIDTH / GRID_SIZE)
#define MAX_ROOMS 11
#define MIN_ROOMS 7

#define MIN(a, b) ((a) < (b) ? (a) : (b))

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
bool gameRestart = false;
bool stairsPlaced = false;

bool CheckRoomCollision(Vector2 a, Vector2 b) {
    return (a.x == b.x && a.y == b.y);
}

void ConnectRooms(int indexA, int indexB) {
    Vector2 a = (Vector2){rooms[indexA].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[indexA].position.y * ROOM_SIZE + ROOM_SIZE / 2};
    Vector2 b = (Vector2){rooms[indexB].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[indexB].position.y * ROOM_SIZE + ROOM_SIZE / 2};

    if (a.x != b.x) {
        DrawRectangle(MIN(a.x, b.x), a.y - 5, abs(b.x - a.x), 10, WHITE);
    }
    if (a.y != b.y) {
        DrawRectangle(b.x - 5, MIN(a.y, b.y), 10, abs(b.y - a.y), WHITE);
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

        for (int j = 0; j < i; j++) {
            if (CheckRoomCollision(rooms[i].position, rooms[j].position)) {
                i--;  // Retry if collision happens
                break;
            }
        }
    }

    rooms[0].visited = true; // Start room
    playerPos = (Vector2){rooms[0].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[0].position.y * ROOM_SIZE + ROOM_SIZE / 2};

    for (int i = 1; i < totalRooms; i++) {
        int connectedRoom = GetRandomValue(0, i - 1);
        if (rooms[connectedRoom].corridorConnections < 3 && rooms[i].corridorConnections < 3) {
            rooms[connectedRoom].connectedTo[rooms[connectedRoom].corridorConnections] = i;
            rooms[i].connectedTo[rooms[i].corridorConnections] = connectedRoom;
            rooms[connectedRoom].corridorConnections++;
            rooms[i].corridorConnections++;
        }
    }

    int distanceRooms = GetRandomValue(4, totalRooms - 1);
    stairsPos = (Vector2){rooms[distanceRooms].position.x * ROOM_SIZE + ROOM_SIZE / 2, rooms[distanceRooms].position.y * ROOM_SIZE + ROOM_SIZE / 2};
    stairsPlaced = true;
}

void DrawDungeon() {
    for (int i = 0; i < totalRooms; i++) {
        Rectangle room = {rooms[i].position.x * ROOM_SIZE, rooms[i].position.y * ROOM_SIZE, ROOM_SIZE, ROOM_SIZE};
        DrawRectangleRec(room, LIGHTGRAY);
        for (int j = 0; j < rooms[i].corridorConnections; j++) {
            if (rooms[i].connectedTo[j] != -1) {
                ConnectRooms(i, rooms[i].connectedTo[j]);
            }
        }
    }

    DrawCircleV(playerPos, 10, BLUE);
    if (stairsPlaced) DrawCircleV(stairsPos, 10, RED);
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
        if (IsKeyPressed(KEY_P)) gamePaused = !gamePaused;
        if (gamePaused) {
            if (IsKeyPressed(KEY_R)) {
                gameRestart = true;
                gamePaused = false;
            }
            if (IsKeyPressed(KEY_ESCAPE)) break;
        }

        if (gameRestart) {
            GenerateDungeon();
            gameRestart = false;
        }

        if (!gamePaused) {
            if (IsKeyDown(KEY_RIGHT)) playerPos.x += 2;
            if (IsKeyDown(KEY_LEFT)) playerPos.x -= 2;
            if (IsKeyDown(KEY_DOWN)) playerPos.y += 2;
            if (IsKeyDown(KEY_UP)) playerPos.y -= 2;

            if (CheckCollisionPointCircle(playerPos, stairsPos, 10)) {
                GenerateDungeon();
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawDungeon();

        if (gamePaused) {
            PauseMenu();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
