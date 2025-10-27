#include "gap_buffer.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  gap_buffer *buffer = GbInitFlushBufferFromString((char *)"Hello World");
  GbPrintBufferDebug(buffer);
  GbInsertCursor(buffer, 0, 5);
  GbPrintBufferDebug(buffer);
  /*SetConfigFlags(FLAG_WINDOW_RESIZABLE);
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
  CloseWindow();*/
  return EXIT_SUCCESS;
}
