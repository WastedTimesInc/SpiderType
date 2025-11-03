#include "gap_buffer.h"
#include "text_buffer.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct editor_params_t {
  float text_size;
  float char_spacing;
  float line_spacing;
  Color text_color;
  Color num_color;
  Color cursor_color;
  Color editor_bg_color;
} editor_params;

typedef enum editor_mode_t { NORMAL = 0, INSERT = 1, COMMAND = 2 } editor_mode;

char *LoadStringFromFile(FILE *f, char *path) {
  f = fopen(path, "r");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  char *returnVal = malloc((fsize + 1) * sizeof(char));
  rewind(f);
  fread(returnVal, 1, fsize, f);
  returnVal[fsize] = '\0';
  return returnVal;
}

char **SplitStringsByLine(char *input, unsigned long *num_lines) {
  *num_lines = 1;
  long *lineBreakPositions = malloc(*num_lines * sizeof(long));
  lineBreakPositions[*num_lines - 1] = -1;
  for (int i = 0; i < strlen(input); i++) {
    if (input[i] == '\n') {
      *num_lines += 1;
      lineBreakPositions =
          realloc(lineBreakPositions, *num_lines * sizeof(long));
      lineBreakPositions[*num_lines - 1] = i;
    }
  }
  lineBreakPositions =
      realloc(lineBreakPositions, (*num_lines + 1) * sizeof(long));
  lineBreakPositions[*num_lines] = strlen(input);
  char **stringArr = malloc(*num_lines * sizeof(char *));
  for (int i = 0; i < *num_lines; i++) {
    long stringSize = lineBreakPositions[i + 1] - lineBreakPositions[i] - 1;
    stringArr[i] = malloc((stringSize + 1) * sizeof(char));
    memcpy(stringArr[i], &input[lineBreakPositions[i] + 1],
           stringSize * sizeof(char));
    stringArr[i][stringSize] = '\0';
  }
  return stringArr;
}

int clamp(int bot, int top, int val) {
  if (val > top) {
    return top;
  } else if (val < bot) {
    return bot;
  }
  return val;
}

int main(int argc, char *argv[]) {
  Vector2 cam_pos ={0,0};
  editor_params params;
  params.text_size = 15;
  params.char_spacing = 2;
  params.line_spacing = 2;
  params.text_color = WHITE;
  params.num_color = GRAY;
  params.cursor_color = RED;
  params.editor_bg_color = BLACK;
  editor_mode mainMode = NORMAL;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(720, 480, (char *)"SpiderType");
  while (!IsWindowReady()) {
  };
  SetWindowMaxSize(3480, 2160);
  SetWindowMinSize(200, 100);
  SetTargetFPS(60);
  SetExitKey(KEY_INSERT);
  Font monoFont = LoadFontEx("font.ttf", params.text_size, 0, 250);
  float charWidth =
      MeasureTextEx(monoFont, "0", params.text_size, params.char_spacing).x;
  text_buffer *mainBuffer = TbInitBuffer(1, 10);
  Vector2 cursorPos;
  Vector2 fpsPos;
  int ln_pad = 0;
  while (!WindowShouldClose()) {
    int pressed = GetCharPressed();
    if (mainMode == NORMAL) {
      switch (GetKeyPressed()) {
      case KEY_BACKSPACE:
        TbMoveLeft(mainBuffer);
        break;
      case KEY_DELETE:
        TbDelete(mainBuffer);
        break;
      case KEY_K:
        TbMoveUp(mainBuffer);
        break;
      case KEY_J:
        TbMoveDown(mainBuffer);
        break;
      case KEY_H:
        TbMoveLeft(mainBuffer);
        break;
      case KEY_L:
        TbMoveRight(mainBuffer);
        break;
      case KEY_ENTER:
        TbMoveDown(mainBuffer);
        break;
      case KEY_I:
        mainMode = INSERT;
        break;
      case KEY_SEMICOLON:
        mainMode = COMMAND;
        GbResetBuffer(commandBuffer, 10);
        break;
      default:
        break;
      }
    } else if (mainMode == INSERT) {
      switch (GetKeyPressed()) {
      case KEY_BACKSPACE:
        TbBackspace(mainBuffer);
        break;
      case KEY_DELETE:
        TbDelete(mainBuffer);
        break;
      case KEY_UP:
        TbMoveUp(mainBuffer);
        break;
      case KEY_DOWN:
        TbMoveDown(mainBuffer);
        break;
      case KEY_LEFT:
        TbMoveLeft(mainBuffer);
        break;
      case KEY_RIGHT:
        TbMoveRight(mainBuffer);
        break;
      case KEY_ENTER:
        TbEnter(mainBuffer);
        break;
      case KEY_ESCAPE:
        mainMode = NORMAL;
        break;
      default:
        if (pressed != 0) {
          bool succes = TbInsertChar(mainBuffer, (char)pressed);
        }
        break;
      }
    } else if (mainMode == COMMAND) {
      char *cmdTxt = GbConcatenate(commandBuffer);
      switch (GetKeyPressed()) {
      case KEY_ESCAPE:
        mainMode = NORMAL;
        break;
      case KEY_ENTER:
        if (cmdTxt[0] == 'q') {
          CloseCall = true;
        }
        mainMode = NORMAL;
        break;
      default:
        if (pressed != 0) {
          if (GbInsertChar(commandBuffer, (char)pressed) == false) {

            GbResizeCursor(commandBuffer, 10);
            GbInsertChar(commandBuffer, (char)pressed);
          }
          GbPrintBufferDebug(commandBuffer);
        }
        break;
      }
      free(cmdTxt);
    }

    if ((TbCursorPosition(mainBuffer)) * (charWidth + params.char_spacing) >
        GetRenderWidth() - 20) {
      camOffset.x = ((TbCursorPosition(mainBuffer) + 1) *
                     (charWidth + params.char_spacing)) -
                    GetRenderWidth() + 20;
    } else {
      camOffset.x = 0.0;
    }
    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), WHITE);
    if(20+cursorPos.x-cam_pos.x>GetRenderWidth()){
      cam_pos.x+=12;
    }else if(cursorPos.x-cam_pos.x<20){
      cam_pos.x-=12;
    }
    if(10+cursorPos.y-cam_pos.y>GetRenderHeight()){
      cam_pos.y+=10;
    }else if(cursorPos.y-cam_pos.y<0){
      cam_pos.y-=10;
    }
    for (int i = 0; i < mainBuffer->num_lines; i++) {
      DrawTextEx(
          monoFont, mainBuffer->concat_lines[i],
          (Vector2){20.0 - cam_pos.x + ln_pad,
                    10.0 + ((params.text_size + params.line_spacing) * i) - cam_pos.y},
          params.text_size, params.char_spacing, BLACK);
      DrawRectangle(0,10.0 + ((params.text_size + params.line_spacing) * i) - cam_pos.y,20+ln_pad,20,WHITE);
      char *lineNum = malloc(20 * sizeof(char));
      if (TbLinePosition(mainBuffer) == i) {
        sprintf(lineNum, "%ld", TbLinePosition(mainBuffer) + 1);
      } else {
        long delta = abs(i - (int)TbLinePosition(mainBuffer));
        sprintf(lineNum, "%ld", delta);
      }
      if(strlen(lineNum)*(params.text_size/2+params.char_spacing)>ln_pad){
        ln_pad =strlen(lineNum)*(params.text_size/2+params.char_spacing);
      }
      DrawTextEx(
          monoFont, lineNum,
          (Vector2){3.0, 10.0 + ((params.text_size + params.line_spacing) * i - cam_pos.y)},
          params.text_size, params.char_spacing, BLACK);
      free(lineNum);

      cursorPos.x = 20 + (TbCursorPosition(mainBuffer) *
                          (charWidth + params.char_spacing));
      cursorPos.y = 10 + (TbLinePosition(mainBuffer) *
                          (params.text_size + params.line_spacing));
      DrawTextEx(monoFont, "_", (Vector2){cursorPos.x-cam_pos.x,cursorPos.y-cam_pos.y}, params.text_size,
                 params.char_spacing, BLACK);

      fpsPos.x = GetRenderWidth() - ((charWidth + params.char_spacing) * 8);
      fpsPos.y = GetRenderHeight() - (params.text_size + params.line_spacing);
      int fps = GetFPS();
      char *fpsChar = malloc(10 * sizeof(char));
      sprintf(fpsChar, (char *)"FPS: ");
      sprintf(&fpsChar[5], "%d", fps);
      DrawTextEx(monoFont, fpsChar, fpsPos, params.text_size,
                 params.char_spacing, BLACK);
      free(fpsChar);
    }
    Vector2 fpsPos;
    fpsPos.x = GetRenderWidth() - ((charWidth + params.char_spacing) * 8);
    fpsPos.y = GetRenderHeight() - (params.text_size + params.line_spacing);
    int fps = GetFPS();
    char *fpsChar = malloc(10 * sizeof(char));
    sprintf(fpsChar, (char *)"FPS: ");
    sprintf(&fpsChar[5], "%d", fps);
    DrawTextEx(monoFont, fpsChar, fpsPos, params.text_size, params.char_spacing,
               params.num_color);

    if (mainMode == NORMAL) {
      DrawTextEx(monoFont, "NORMAL", (Vector2){3, fpsPos.y}, params.text_size,
                 params.char_spacing, params.num_color);
    } else if (mainMode == INSERT) {
      DrawTextEx(monoFont, "INSERT", (Vector2){3, fpsPos.y}, params.text_size,
                 params.char_spacing, params.num_color);
    }

    char *commandBufTxt = malloc(64 * sizeof(char));
    switch (mainMode) {
    case NORMAL:
      DrawTextEx(monoFont, "NORMAL", (Vector2){3, fpsPos.y}, params.text_size,
                 params.char_spacing, params.num_color);
      break;
    case INSERT:
      DrawTextEx(monoFont, "INSERT", (Vector2){3, fpsPos.y}, params.text_size,
                 params.char_spacing, params.num_color);
      break;
    case COMMAND:

      sprintf(commandBufTxt, "COMMAND:");
      char *concatCmd = GbConcatenate(commandBuffer);
      sprintf(&commandBufTxt[8], "%s", concatCmd);
      free(concatCmd);
      DrawTextEx(monoFont, commandBufTxt, (Vector2){3, fpsPos.y},
                 params.text_size, params.char_spacing, params.num_color);

      break;
    }
    free(commandBufTxt);
    EndDrawing();
  }
  /* int tot_line = 1;
   int current_line = 0;
   bool *modif = malloc(sizeof(bool));
   modif[0] = false;
   char **concat = malloc(sizeof(char *));
   concat[0] = malloc(1);
   concat[0][0] = '\0';
   int mode = 0;
   bool succes = false;
   int fsize = 0;
   gap_buffer *testBuff = malloc(sizeof(gap_buffer));
   testBuff[current_line].cursor_start = 0;
   testBuff[current_line].end = 9;
   testBuff[current_line].data = malloc(11);
   testBuff[current_line].cursor_end = 9;
   testBuff[current_line].data[10] = '\0';
   int x_offset = 10;
   int cam_offset[2] = {0, 0};
   while (!WindowShouldClose()) {
     int pressed = GetCharPressed();
     switch (GetKeyPressed()) {
     case KEY_BACKSPACE:
       GbBackspace(&testBuff[current_line]);
       modif[current_line] = true;
       break;
     case KEY_DELETE:
       GbDelete(&testBuff[current_line]);
       modif[current_line] = true;
       break;
     case KEY_UP:
       if ((current_line != 0)) {
         GbInsertCursor(&testBuff[current_line - 1],
                        clamp(0, testBuff[current_line - 1].end,
                              testBuff[current_line].cursor_start),
                        10);
         GbFlushBuffer(&testBuff[current_line]);
         current_line--;
         if ((30 + 22 * current_line + cam_offset[1]) < 20) {
           cam_offset[1] += 22;
         }
       }
       break;
     case KEY_DOWN:
       if ((current_line != tot_line - 1)) {
         GbInsertCursor(&testBuff[current_line + 1],
                        clamp(0, testBuff[current_line + 1].end,
                              testBuff[current_line].cursor_start),
                        10);
         GbFlushBuffer(&testBuff[current_line]);
         current_line++;
         if ((30 + 22 * current_line + cam_offset[1]) > GetRenderHeight() - 20)
   { cam_offset[1] -= 22;
         }
       }
       break;
     case KEY_LEFT:
       if (!GbMoveLeft(&testBuff[current_line]) && (current_line != 0)) {
         GbFlushBuffer(&testBuff[current_line]);
         GbInsertCursor(&testBuff[current_line - 1],
                        testBuff[current_line - 1].end + 1, 10);
         current_line -= 1;
         if ((30 + 22 * current_line + cam_offset[1]) < 20) {
           cam_offset[1] += 22;
         }
         cam_offset[0] = 0;
       } else {
         if (cam_offset[0] + 10 + x_offset +
                 MeasureText(testBuff[current_line].data, 20) -
                 MeasureText(testBuff[current_line].data +
                                 testBuff[current_line].cursor_start,
                             20) <
             20) {
           cam_offset[0] += 20;
         }
       }
       break;
     case KEY_RIGHT:
       if (!GbMoveRight(&testBuff[current_line]) &&
           (current_line != (tot_line - 1))) {
         GbFlushBuffer(&testBuff[current_line]);
         GbInsertCursor(&testBuff[current_line + 1],
                        testBuff[current_line + 1].end + 1, 10);
         current_line += 1;
         if ((30 + 22 * current_line + cam_offset[1]) > GetRenderHeight() - 20)
   { cam_offset[1] -= 22;
         }
         cam_offset[0] = 0;
       } else {
         if (cam_offset[0] + 10 + x_offset +
                 MeasureText(testBuff[current_line].data, 20) -
                 MeasureText(testBuff[current_line].data +
                                 testBuff[current_line].cursor_start,
                             20) >
             GetRenderWidth() - 20) {
           cam_offset[0] -= 20;
         }
       }
       break;
     case KEY_ENTER:
       GbFlushBuffer(&testBuff[current_line]);
       testBuff = realloc(testBuff, (tot_line + 1) * sizeof(gap_buffer));
       concat = realloc(concat, (tot_line + 1) * sizeof(char *));
       modif = realloc(modif, (tot_line + 1) * sizeof(bool));
       memcpy(concat + current_line + 2, concat + current_line + 1,
              (tot_line - current_line - 1) * sizeof(char *));
       concat[current_line + 1] = malloc(1);
       concat[current_line + 1][0] = '\0';
       memcpy(modif + current_line + 2, modif + current_line + 1,
              (tot_line - current_line - 1) * sizeof(bool));
       modif[current_line + 1] = true;
       memcpy(testBuff + current_line + 2, testBuff + current_line + 1,
              (tot_line - current_line - 1) * sizeof(gap_buffer));
       testBuff[current_line + 1].cursor_start = 0;
       testBuff[current_line + 1].end = 9;
       testBuff[current_line + 1].data = calloc(11, sizeof(char));
       testBuff[current_line + 1].cursor_end = 9;
       testBuff[current_line + 1].data[10] = '\0';
       tot_line++;
       x_offset = MeasureText(TextFormat("%i", tot_line), 10);
       current_line++;
       cam_offset[0] = 0;
       if ((30 + 22 * current_line + cam_offset[1]) > GetRenderHeight() - 20) {
         cam_offset[1] -= 22;
       }
       break;
     case KEY_LEFT_CONTROL:
       mode += 1;
     default:
       if (pressed != 0) {
         switch (mode) {
         case 1:
           switch (pressed) { // Switch for the CTRL+KEY bindings
           case 's':
             break;
           default:
             mode -= 1;
             break;
           }
           break;
         default:
           succes = GbInsertChar(&testBuff[current_line], (char)pressed);
           if (!succes) {
             GbResizeCursor(&testBuff[current_line], 10);
             succes = GbInsertChar(&testBuff[current_line], (char)pressed);
           }
           if (!succes) {
             perror("Error : Failed to add char after resizing!\n");
           }
           modif[current_line] = true;
           break;
         }
       }
       break;
     }

     BeginDrawing();
     DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), WHITE);
     for (int i = 0; i < tot_line; i++) {
       if (i != current_line) {
         DrawText(TextFormat("%i", ((i - current_line) * (i > current_line) +
                                    (current_line - i) * (i < current_line))),
                  0, cam_offset[1] + 35 + i * 22, 10, BLACK);
       }
       if (modif[i]) {
         free(concat[i]);
         concat[i] = GbConcatenate(&testBuff[i]);
         if (cam_offset[0] + 10 + x_offset +
                 MeasureText(testBuff[current_line].data, 20) -
                 MeasureText(testBuff[current_line].data +
                                 testBuff[current_line].cursor_start,
                             20) >
             GetRenderWidth() - 20) {
           cam_offset[0] -= 20;
         }
         DrawText(concat[i], cam_offset[0] + 10 + x_offset,
                  cam_offset[1] + 30 + i * 22, 20, BLACK);
         modif[i] = false;
       } else {
         DrawText(concat[i], cam_offset[0] + 10 + x_offset,
                  cam_offset[1] + 30 + i * 22, 20, BLACK);
       }
     }
     DrawText("_",
              cam_offset[0] + 10 + x_offset +
                  MeasureText(testBuff[current_line].data, 20) -
                  MeasureText(testBuff[current_line].data +
                                  testBuff[current_line].cursor_start,
                              20),
              cam_offset[1] + 32 + current_line * 22, 20, BLUE);
     DrawText(TextFormat("%i", current_line), 0,
              cam_offset[1] + 35 + current_line * 22, 10, RED);
     EndDrawing();
   }
   CloseWindow();
   for (int i = 0; i < tot_line; i++) {
     GbDestroy(&testBuff[i]);
   }
   free(testBuff);*/
  CloseWindow();
  UnloadFont(monoFont);
  GbDestroy(commandBuffer);
  free(commandBuffer);
  TbDestroy(mainBuffer);
  free(mainBuffer);
  return EXIT_SUCCESS;
}
