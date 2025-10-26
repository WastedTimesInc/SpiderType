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

bool GapBufferInsert(gap_buffer *buffer, char insert) {
  if (buffer->cursor_end - buffer->cursor_start > 0) {
    buffer->data[buffer->cursor_start] = insert;
    buffer->cursor_start++;
    return true;
  }
  return false;
}

bool GapBufferInsertArray(gap_buffer *buffer, char *insert) {
  if (strlen(insert) < buffer->cursor_end - buffer->cursor_start + 1) {
    strcpy(&buffer->data[buffer->cursor_start], insert);
    buffer->cursor_start += strlen(insert);
    return true;
  }
  return false;
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

gap_buffer *GapBufferCopy(gap_buffer *buffer) {
  gap_buffer *bufferCache = malloc(sizeof(gap_buffer));
  memcpy(bufferCache, buffer, sizeof(gap_buffer));
  bufferCache->data = malloc((bufferCache->end + 1) * sizeof(char));
  memcpy(bufferCache->data, buffer->data,
         (bufferCache->end + 1) * sizeof(char));
  return bufferCache;
}

void GapBufferResizeGap(gap_buffer *buffer, unsigned int gap_size) {
  int cacheCursorSize = buffer->cursor_end - buffer->cursor_start + 1;
  int cursorDelta = gap_size - cacheCursorSize;
  gap_buffer *bufferCache = GapBufferCopy(buffer);
  GapBufferPrintBufferDebug(bufferCache);

  free(buffer->data);
  buffer->cursor_end += cursorDelta;
  buffer->end += cursorDelta;
  buffer->data = malloc((buffer->end + 1) * sizeof(char));
  if (buffer->cursor_start - buffer->start > 0) {
    memcpy(buffer->data, bufferCache->data,
           (buffer->cursor_start - buffer->start) * sizeof(char));
  }
  if (buffer->end - buffer->cursor_end > 0) {
    memcpy(&buffer->data[buffer->cursor_end + 1],
           &bufferCache->data[bufferCache->cursor_end + 1],
           buffer->end - buffer->cursor_end + 1);
  }
}

void GapBufferResizeGapDebug(gap_buffer *buffer, unsigned int gap_size) {
  GapBufferResizeGap(buffer, gap_size);
  for (int i = buffer->cursor_start; i <= buffer->cursor_end; i++) {
    buffer->data[i] = '-';
  }
}

char *GapBufferConcatenate(gap_buffer *buffer) {
  char *concat = malloc((buffer->cursor_start - buffer->start + buffer->end -
                         buffer->cursor_end + 1) *
                        sizeof(char));
  if (buffer->cursor_start - buffer->start > 0) {
    memcpy(concat, buffer->data,
           (buffer->cursor_start - buffer->start) * sizeof(char));
  }
  if (buffer->end - buffer->cursor_end > 0) {
    memcpy(&concat[buffer->cursor_start], &buffer->data[buffer->cursor_end + 1],
           buffer->end - buffer->cursor_end + 1);
  }
  concat[buffer->cursor_start - buffer->start + buffer->end -
         buffer->cursor_end] = '\0';
  return concat;
}

int GapBufferGapSize(gap_buffer *buffer) {
  return buffer->cursor_end - buffer->cursor_start + 1;
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
  GapBufferInit(&testBuff, 100);

  while (!WindowShouldClose()) {
    int pressed = GetCharPressed();
    if (pressed != 0) {
      GapBufferInsert(&testBuff, (char)pressed);
    }
    if (GapBufferGapSize(&testBuff) < 10) {
      GapBufferResizeGap(&testBuff, 100);
    }
    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), WHITE);
    DrawText((char *)"SpiderType", 10, 10, 20, BLACK);
    DrawText(GapBufferConcatenate(&testBuff), 10, 30, 20, BLACK);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
