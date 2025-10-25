#include <bits/pthreadtypes.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct gap_buffer_t {
  unsigned int start;
  unsigned int end;
  unsigned int cursor_start; // relative to buffer start
  unsigned int cursor_end; // Seems redundant
  char *data;
} gap_buffer;

void GapBufferInit(gap_buffer *buffer, unsigned int req_size) {
  buffer->data = malloc((req_size+1) * sizeof(char));
  buffer->data[req_size]='\0';
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
  if(buffer->cursor_start==(buffer->end-buffer->start)){
    buffer->data = realloc(buffer->data,1+(buffer->end-buffer->start)*2);
    buffer->data[(buffer->end-buffer->start)*2]='\0';
    buffer->end=buffer->start+1+(buffer->end-buffer->start)*1.5;
    buffer->cursor_end=buffer->start+1+(buffer->end-buffer->start)*1.5;
  }
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
  char* latch = calloc(100,sizeof(char));
  int latchSize =0;
  int textSize =0;
  int fontSize = 10;
  int cursorLocation[2]={0,0};
  char* txtBuff;
  char c ='\0';
  float width_factor = 0.90;
  gap_buffer* inBuff = malloc(sizeof(gap_buffer));
  GapBufferInitDebug(inBuff, 1);
  GapBufferInitDebug(&testBuff, 10);
  GapBufferPrintBufferDebug(&testBuff);
  GapBufferInsert(&testBuff, 'A');
  GapBufferPrintBufferDebug(&testBuff);
  GapBufferMoveLeft(&testBuff, 1);
  GapBufferPrintBufferDebug(&testBuff);
  while (!WindowShouldClose()) {
    c = GetCharPressed();
    while(c!=0){
      GapBufferInsert(inBuff,c);
      cursorLocation[0]+=1;
      textSize++;
      c = GetCharPressed();
    }
    if(IsKeyPressed(KEY_LEFT)){
      cursorLocation[0]+=-1+(cursorLocation[0]==0);
    }else if(IsKeyPressed(KEY_RIGHT)){
      cursorLocation[0]+=1-(cursorLocation[0]>textSize);
    }

    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), BLACK);
    DrawText(inBuff->data,0,0,fontSize,GRAY);
    DrawText("_",MeasureText(inBuff->data,fontSize)-MeasureText(inBuff->data+cursorLocation[0],fontSize),fontSize*cursorLocation[1]+1,fontSize,ORANGE);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
