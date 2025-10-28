#include "gap_buffer.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[]) {
  FILE *f = NULL;
  char *loadStr;
  if (argc > 1) {
    loadStr = LoadStringFromFile(f, argv[1]);
    unsigned long num_lines;
    char **splitLines = SplitStringsByLine(loadStr, &num_lines);
    gap_buffer **textBuffer = malloc((num_lines) * sizeof(gap_buffer));
    for (int i = 0; i < num_lines; i++) {
      textBuffer[i] = GbInitFlushBufferFromString(splitLines[i]);
      GbPrintBufferDebug(textBuffer[i]);
    }
  }
  /*SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(720, 480, (char *)"SpiderType");
  while (!IsWindowReady()) {
  };
  SetWindowMaxSize(3480, 2160);
  SetWindowMinSize(200, 100);
  SetTargetFPS(60);
  printf("Window ready\n");
  int tot_line = 0;
  int current_line = 0;
  char *concat;
  int mode = 0;
  int fsize = 0;
  gap_buffer testBuff;
  int x_offset = 10;
  FILE *f = NULL;
  if (argc > 1) {
    f = fopen(argv[1], "rw");
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    GapBufferInit(&testBuff, fsize);
    testBuff.cursor_start += fsize - 1;
    fseek(f, 0, SEEK_SET);
    fscanf(f, "%s", testBuff.data);
    for (int i = 0; i < fsize; i++) {
      if (testBuff.data[i] == '\n') {
        tot_line++;
      }
    }

  } else {
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
      if (current_line != 0 && testBuff.data[testBuff.cursor_start] == '\n') {
        current_line--;
      }
      GapBufferMoveLeft(&testBuff, 1);
      break;
    case KEY_RIGHT:
      if (current_line != tot_line &&
          testBuff.data[testBuff.cursor_start] == '\n') {
        current_line++;
      }
      GapBufferMoveRight(&testBuff, 1);
      break;
    case KEY_ENTER:
      GapBufferInsert(&testBuff, '\n');
      tot_line++;
      x_offset = MeasureText(TextFormat("%i", tot_line), 10);
      current_line++;
      break;
    case KEY_LEFT_CONTROL:
      mode += 1;
    default:
      if (pressed != 0) {
        switch (mode) {
        case 1:
          switch (pressed) { // Switch for the CTRL+KEY bindings
          case 's':
            if (f == NULL) {
              // TODO Handle queriying the user for a new file
            } else {
              concat = GapBufferConcatenate(&testBuff);
              fprintf(f, "%s", concat);
              free(concat);
              mode -= 1;
            }
            break;
          default:
            mode -= 1;
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
    DrawText(concat, 10 + x_offset, 30, 20, BLACK);

    for (int i = 0; i <= tot_line; i++) {
      if (i != current_line) {
        DrawText(TextFormat("%i", ((i - current_line) * (i > current_line) +
                                   (current_line - i) * (i < current_line))),
                 0, 35 + i * 22, 10, BLACK);
      }
    }
    DrawText(TextFormat("%i", current_line), 0, 35 + current_line * 22, 10,
             RED);
    free(concat);
    EndDrawing();
  }
  CloseWindow();*/
  return EXIT_SUCCESS;
}
