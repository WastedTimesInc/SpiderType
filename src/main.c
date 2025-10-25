#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  InitWindow(720, 480, (char *)"SpiderType");
  while (!IsWindowReady())
    ;
  printf("Window ready");
  BeginDrawing();
  DrawRectangle(0, 0, 720, 480, WHITE);
  DrawText((char *)"SpiderType", 10, 10, 20, BLACK);
  EndDrawing();
  while (true)
    ;
  return EXIT_SUCCESS;
}
