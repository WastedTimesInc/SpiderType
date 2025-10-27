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
void GapBufferResizeGap(gap_buffer *buffer, unsigned int gap_size) {
  int cacheCursorSize = buffer->cursor_end - buffer->cursor_start;
  int cursorDelta = gap_size - cacheCursorSize;
  buffer->data = realloc(buffer->data,(buffer->end +cursorDelta + 1) * sizeof(char));
  if (buffer->end - buffer->cursor_end > 0) {
    memcpy(&buffer->data[buffer->cursor_end + cursorDelta],
           &buffer->data[buffer->cursor_end ],
           buffer->end - buffer->cursor_end + 1);
  }
  buffer->cursor_end += cursorDelta;
  buffer->end += cursorDelta;
}

bool GapBufferInsert(gap_buffer *buffer, char insert) {
  if (buffer->cursor_end - buffer->cursor_start > 0) {
    buffer->data[buffer->cursor_start] = insert;
    buffer->cursor_start++;
    return true;
  }else{
    GapBufferResizeGap(buffer,10);
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

//Refactoring this to avoid useless memcpys

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

bool GapBufferBackspace(gap_buffer *buffer) {
  if (buffer->cursor_start > buffer->start) {
    buffer->cursor_start--;
    return true;
  }
  return false;
}

bool GapBufferDelete(gap_buffer *buffer) {
  if (buffer->cursor_end < buffer->end) {
    buffer->cursor_end++;
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
  int tot_line =0;
  int current_line =0;
  char *concat;
  int mode = 0;
  int fsize = 0;
  gap_buffer testBuff;
  int x_offset =10;
  FILE* f = NULL;
  if(argc>1){
    f = fopen(argv[1],"rw");
    fseek(f,0,SEEK_END);
    fsize=ftell(f);
    GapBufferInit(&testBuff, fsize);
    testBuff.cursor_start+=fsize-1;
    fseek(f,0,SEEK_SET);
    fscanf(f,"%s",testBuff.data);
    for(int i=0;i<fsize;i++){
      if(testBuff.data[i]=='\n'){
        tot_line++;
      }
    }

  }else{
    GapBufferInit(&testBuff, 10);
  }
  while (!WindowShouldClose()) {
    int pressed = GetCharPressed();
    switch (GetKeyPressed()) {
    case KEY_BACKSPACE:
      GapBufferBackspace(&testBuff);
      break;
    case KEY_DELETE:
      GapBufferDelete(&testBuff);
      break;
    case KEY_LEFT:
      if(current_line!=0&&testBuff.data[testBuff.cursor_start]=='\n'){
      current_line--;
      }
      GapBufferMoveLeft(&testBuff, 1);
      break;
    case KEY_RIGHT:
      if(current_line!=tot_line&&testBuff.data[testBuff.cursor_start]=='\n'){
        current_line++;
      }
      GapBufferMoveRight(&testBuff, 1);
      break;
    case KEY_ENTER:
      GapBufferInsert(&testBuff,'\n');
      tot_line++;
      x_offset=MeasureText(TextFormat("%i",tot_line),10);
      current_line++;
      break;
    case KEY_LEFT_CONTROL:
      mode +=1;
    default:
      if (pressed != 0) {
        switch (mode){
            case 1:
              switch(pressed){ //Switch for the CTRL+KEY bindings
                case 's':
                  if(f==NULL){
                    //TODO Handle queriying the user for a new file
                  }else{
                    concat = GapBufferConcatenate(&testBuff);
                    fprintf(f,"%s",concat);
                    free(concat);
                    mode-=1;
                  }
                  break;
                default:
                  mode-=1;
                  break;
              }
              break;
            default:
              GapBufferInsert(&testBuff, (char)pressed);
              break;
          }
      }
      break;
    }

    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), WHITE);
    DrawText((char *)"SpiderType", 10, 10, 20, BLACK);
    concat = GapBufferConcatenate(&testBuff);
    DrawText(concat, 10+x_offset, 30, 20, BLACK);

    for(int i =0;i<=tot_line;i++){
      if(i!=current_line){
        DrawText(TextFormat("%i",((i-current_line)*(i>current_line)+(current_line-i)*(i<current_line))),0,35+i*22,10,BLACK);
      }
    }
    DrawText(TextFormat("%i",current_line),0,35+current_line*22,10,RED);
    free(concat);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
