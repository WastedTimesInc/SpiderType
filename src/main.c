#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(720, 480, (char *)"SpiderType");
  while (!IsWindowReady()) {
  };
  SetWindowMaxSize(3480, 2160);
  SetWindowMinSize(200, 100);
  SetTargetFPS(60);
  printf("Window ready");
  while (!WindowShouldClose()) {
    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), WHITE);
    DrawText((char *)"SpiderType", 10, 10, 20, BLACK);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
