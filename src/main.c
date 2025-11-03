#include "gap_buffer.h"
#include "text_buffer.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct editor_params_t {
  float text_size;
  float char_width;
  float char_spacing;
  float line_spacing;
  float line_num_padding;
  float top_offset;
  float left_offset;
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

int numPlaces(int n) {
  if (n == 0)
    return 1;
  return floor(log10(abs(n))) + 1;
}

int main(int argc, char *argv[]) {
  // Raylib config flags
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(720, 480, (char *)"SpiderType");
  while (!IsWindowReady()) {
  };
  SetWindowMaxSize(3480, 2160);
  SetWindowMinSize(200, 100);
  SetTargetFPS(60);
  SetExitKey(KEY_INSERT);

  // Editor variables and parameters setup
  Vector2 cam_pos = {0, 0};
  editor_params params;
  params.text_size = 15;
  params.char_spacing = 2;
  params.line_spacing = 2;
  params.line_num_padding = 0;
  params.top_offset = 5;
  params.left_offset = 5;
  params.text_color = WHITE;
  params.num_color = GRAY;
  params.cursor_color = RED;
  params.editor_bg_color = BLACK;
  editor_mode mainMode = NORMAL;
  Font monoFont = LoadFontEx("font.ttf", params.text_size, 0, 250);
  params.char_width =
      MeasureTextEx(monoFont, "0", params.text_size, params.char_spacing).x;
  text_buffer *mainBuffer = TbInitBuffer(1, 10);
  Vector2 cursorPos = {0, 0};
  int ln_pad = 0;
  gap_buffer *commandBuffer = GbInitBuffer(10);
  bool CloseCall = false;
  while (!WindowShouldClose() && !CloseCall) {
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
    int delta1 = TbLinePosition(mainBuffer);
    int delta2 = mainBuffer->num_lines - 1 - TbLinePosition(mainBuffer);
    if (delta1 >= delta2) {
      params.line_num_padding =
          numPlaces(delta1) * (params.char_width + params.char_spacing);
      printf("%d\n", numPlaces(delta1));
    } else {
      params.line_num_padding =
          numPlaces(delta2) * (params.char_width + params.char_spacing);

      printf("%d\n", numPlaces(delta2));
    }
    if (params.left_offset + params.line_num_padding + cursorPos.x - cam_pos.x >
        GetRenderWidth()) {
      cam_pos.x += params.char_width + params.char_spacing;
    } else if (cursorPos.x - cam_pos.x <
               params.left_offset + params.line_num_padding) {
      cam_pos.x -= params.char_width + params.char_spacing;
    }
    if (params.top_offset + cursorPos.y - cam_pos.y > GetRenderHeight()) {
      cam_pos.y += params.text_size + params.line_spacing;
    } else if (cursorPos.y - cam_pos.y < 0) {
      cam_pos.y -= params.text_size + params.line_spacing;
    }
    printf("%f\n", cam_pos.x);
    printf("%f\n", cam_pos.y);

    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(),
                  params.editor_bg_color);
    for (int i = 0; i < mainBuffer->num_lines; i++) {
      Vector2 textPos = {
          params.left_offset + params.line_num_padding - cam_pos.x,
          params.top_offset + ((params.text_size + params.char_spacing) * i) -
              cam_pos.y};
      DrawTextEx(monoFont, mainBuffer->concat_lines[i], textPos,
                 params.text_size, params.char_spacing, params.text_color);

      char *lineNum = malloc(20 * sizeof(char));
      if (TbLinePosition(mainBuffer) == i) {
        sprintf(lineNum, "%ld", TbLinePosition(mainBuffer) + 1);
      } else {
        long delta = abs(i - (int)TbLinePosition(mainBuffer));
        sprintf(lineNum, "%ld", delta);
      }
      DrawTextEx(monoFont, lineNum,
                 (Vector2){params.left_offset,
                           params.top_offset +
                               ((params.text_size + params.line_spacing) * i -
                                cam_pos.y)},
                 params.text_size, params.char_spacing, params.num_color);
      free(lineNum);
    }
    cursorPos.x = params.left_offset + params.line_num_padding +
                  (TbCursorPosition(mainBuffer) *
                   (params.char_width + params.char_spacing));
    cursorPos.y =
        params.top_offset +
        (TbLinePosition(mainBuffer) * (params.text_size + params.line_spacing));
    DrawTextEx(monoFont, "_",
               (Vector2){cursorPos.x - cam_pos.x, cursorPos.y - cam_pos.y},
               params.text_size, params.char_spacing, params.cursor_color);

    char *commandBufTxt = malloc(64 * sizeof(char));
    switch (mainMode) {
    case NORMAL:
      DrawTextEx(monoFont, "NORMAL",
                 (Vector2){3, GetRenderHeight() -
                                  (params.text_size + params.line_spacing)},
                 params.text_size, params.char_spacing, params.num_color);
      break;
    case INSERT:
      DrawTextEx(monoFont, "INSERT",
                 (Vector2){3, GetRenderHeight() -
                                  (params.text_size + params.line_spacing)},
                 params.text_size, params.char_spacing, params.num_color);
      break;
    case COMMAND:

      sprintf(commandBufTxt, "COMMAND:");
      char *concatCmd = GbConcatenate(commandBuffer);
      sprintf(&commandBufTxt[8], "%s", concatCmd);
      free(concatCmd);
      DrawTextEx(monoFont, commandBufTxt,
                 (Vector2){3, GetRenderHeight() -
                                  (params.text_size + params.line_spacing)},
                 params.text_size, params.char_spacing, params.num_color);

      break;
    }
    free(commandBufTxt);
    EndDrawing();
  }
  CloseWindow();
  UnloadFont(monoFont);
  GbDestroy(commandBuffer);
  free(commandBuffer);
  TbDestroy(mainBuffer);
  free(mainBuffer);
  return EXIT_SUCCESS;
}
