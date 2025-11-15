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
  float line_num_margin;
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
  params.line_num_margin = 6;
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
      // vim-style ':' (shift+semicolon) enters COMMAND mode
      if (pressed == ':') {
        mainMode = COMMAND;
        GbResetBuffer(commandBuffer, 10);
      }

      // Movement / edit keys with repeat
      if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        TbMoveLeft(mainBuffer);
      } else if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        TbDelete(mainBuffer);
      } else if (IsKeyPressed(KEY_K) || IsKeyPressedRepeat(KEY_K)) {
        TbMoveUp(mainBuffer);
      } else if (IsKeyPressed(KEY_J) || IsKeyPressedRepeat(KEY_J)) {
        TbMoveDown(mainBuffer);
      } else if (IsKeyPressed(KEY_H) || IsKeyPressedRepeat(KEY_H)) {
        TbMoveLeft(mainBuffer);
      } else if (IsKeyPressed(KEY_L) || IsKeyPressedRepeat(KEY_L)) {
        TbMoveRight(mainBuffer);
      } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) {
        TbMoveDown(mainBuffer);
      }

      // Mode change: 'i' (no repeat)
      if (IsKeyPressed(KEY_I)) {
        mainMode = INSERT;
      }

    } else if (mainMode == INSERT) {
      // Navigation / edit keys with repeat
      if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        TbBackspace(mainBuffer);
      } else if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        TbDelete(mainBuffer);
      } else if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
        TbMoveUp(mainBuffer);
      } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
        TbMoveDown(mainBuffer);
      } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        TbMoveLeft(mainBuffer);
      } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        TbMoveRight(mainBuffer);
      } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) {
        TbEnter(mainBuffer);
      }

      if (IsKeyPressed(KEY_ESCAPE)) {
        mainMode = NORMAL;
      } else if (pressed != 0) {
        // Text insertion: GetCharPressed() already respects OS key repeat
        TbInsertChar(mainBuffer, (char)pressed);
      }

    } else if (mainMode == COMMAND) {
      char *cmdTxt = GbConcatenate(commandBuffer);

      if (IsKeyPressed(KEY_ESCAPE)) {
        mainMode = NORMAL;
      } else if (IsKeyPressed(KEY_ENTER)) {
        if (cmdTxt[0] == 'q') {
          CloseCall = true;
        }
        mainMode = NORMAL;
      } else if (pressed != 0) {
        if (GbInsertChar(commandBuffer, (char)pressed) == false) {
          GbResizeCursor(commandBuffer, 10);
          GbInsertChar(commandBuffer, (char)pressed);
        }
        GbPrintBufferDebug(commandBuffer);
      }

      free(cmdTxt);
    }

    int delta1 = TbLinePosition(mainBuffer);
    int delta2 = mainBuffer->num_lines - 1 - TbLinePosition(mainBuffer);
    if (delta1 >= delta2) {
      params.line_num_padding =
          numPlaces(delta1) * (params.char_width + params.char_spacing) +
          params.line_num_margin;
      printf("%d\n", numPlaces(delta1));
    } else {
      params.line_num_padding =
          numPlaces(delta2) * (params.char_width + params.char_spacing) +
          params.line_num_margin;

      printf("%d\n", numPlaces(delta2));
    }

    cursorPos.x = params.left_offset + params.line_num_padding +
                  (TbCursorPosition(mainBuffer) *
                   (params.char_width + params.char_spacing));
    cursorPos.y =
        params.top_offset +
        (TbLinePosition(mainBuffer) * (params.text_size + params.line_spacing));

    float visible_cursor_x = cursorPos.x - cam_pos.x;
    float left_bound = params.left_offset + params.line_num_padding;

    // Scroll right when the *right edge* of the cursor would go off screen
    if (visible_cursor_x + params.char_width > GetRenderWidth()) {
      cam_pos.x += params.char_width + params.char_spacing;
    }
    // Scroll left when the cursor origin crosses the left bound
    else if (visible_cursor_x < left_bound) {
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
