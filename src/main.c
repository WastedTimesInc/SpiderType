#include <bits/pthreadtypes.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct gap_buffer_ {
  unsigned int start;
  unsigned int end;
  unsigned int cursor_start;
  unsigned int cursor_end;
  char *data;
} gap_buffer;

void GapBufferInit(gap_buffer *buffer, unsigned int req_size) {
  buffer->data = malloc(req_size * sizeof(char));
  buffer->start = 0;
  buffer->end = req_size - 1;
  buffer->cursor_start = buffer->start;
  buffer->cursor_end = buffer->end;
}

void GapBufferInitDebug(gap_buffer *buffer, unsigned int req_size) {
  GapBufferInit(buffer, req_size);

  for (int i = 0; i < req_size; i++) {
    buffer->data[i] = '-';
  }
}

void GapBufferDestroy(gap_buffer *buffer) { free(buffer->data); }

void GapBufferInsert(gap_buffer *buffer, char insert) {
  buffer->data[buffer->cursor_start] = insert;
  buffer->cursor_start++;
}

void GapBufferPrintBufferDebug(gap_buffer *buffer) {
  for (int i = buffer->start; i <= buffer->end; i++) {
    char printChar = buffer->data[i];
    printf("%s", &printChar);
  }
  printf("\n");
  for (int i = buffer->start; i < buffer->cursor_start; i++) {
    printf((char *)" ");
  }
  printf((char *)"C");
  for (int i = buffer->cursor_start + 1; i < buffer->cursor_end; i++) {
    printf((char *)" ");
  }
  printf((char *)"E");
  for (int i = buffer->cursor_end + 1; i <= buffer->end; i++) {
    printf((char *)" ");
  }
  printf("\n");
}

bool GapBufferMoveLeft(gap_buffer *buffer, unsigned int amount) {
  if (amount <= buffer->cursor_start) {
    for (int i = 0; i < amount; i++) {
      buffer->data[buffer->cursor_end] = buffer->data[buffer->cursor_start - 1];
      buffer->cursor_start--;
      buffer->cursor_end--;
    }
    return true;
  }
  return false;
}

bool GapBufferMoveRight(gap_buffer *buffer, unsigned int amount) {
  if (amount <= buffer->end - buffer->cursor_end) {
    for (int i = 0; i < amount; i++) {
      buffer->data[buffer->cursor_start] = buffer->data[buffer->cursor_end + 1];
      buffer->cursor_start++;
      buffer->cursor_end++;
    }
    return true;
  }
  return false;
}

int main(int argc, char *argv[]) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(720, 480, (char *)"SpiderType");
  while (!IsWindowReady()) {
  };
  SetWindowMaxSize(3480, 2160);
  SetWindowMinSize(200, 100);
  SetTargetFPS(60);
  printf("Window ready\n");
  gap_buffer testBuff;
  GapBufferInitDebug(&testBuff, 10);
  GapBufferPrintBufferDebug(&testBuff);
  GapBufferInsert(&testBuff, 'A');
  GapBufferPrintBufferDebug(&testBuff);
  GapBufferMoveLeft(&testBuff, 1);
  GapBufferPrintBufferDebug(&testBuff);
  while (!WindowShouldClose()) {
    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), WHITE);
    DrawText((char *)"SpiderType", 10, 10, 20, BLACK);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
