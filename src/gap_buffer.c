#include "gap_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gap_buffer *GbInitEmptyBuffer() {
  gap_buffer *buffer = malloc(sizeof(gap_buffer));
  buffer->cursor_start = -1;
  buffer->cursor_end = -1;
  buffer->end = -1;
  buffer->data = malloc(sizeof(char));
  buffer->data[0] = '\0';
  return buffer;
}

gap_buffer *GbInitBuffer(long req_size) {
  gap_buffer *buffer = malloc(sizeof(gap_buffer));
  buffer->cursor_start = 0;
  buffer->end = req_size - 1;
  buffer->cursor_end = buffer->end;
  buffer->data = malloc((req_size + 1) * sizeof(char));
  buffer->data[req_size] = '\0';
  return buffer;
}

gap_buffer *GbInitFlushBufferFromString(char *input) {
  gap_buffer *buffer = malloc(sizeof(gap_buffer));
  buffer->cursor_start = -1;
  buffer->cursor_end = -1;
  long inputLength = strlen(input);
  buffer->end = inputLength - 1;
  buffer->data = malloc((inputLength + 1) * sizeof(char));
  strcpy(buffer->data, input);
  buffer->data[inputLength] = '\0';
  return buffer;
}

void GbDestroy(gap_buffer *buffer) {
  free(buffer->data);
  free(buffer);
}

bool GbIsFlushed(gap_buffer *buffer) {
  if (buffer->cursor_start == -1 && buffer->cursor_end == -1) {
    return true;
  }
  return false;
}

long GbCursorSize(gap_buffer *buffer) {
  return (buffer->cursor_end - buffer->cursor_start + 1);
}

long GbTextSize(gap_buffer *buffer) {
  if (!GbIsFlushed(buffer)) {
    return (buffer->end - buffer->cursor_end + buffer->cursor_start);
  }
  return (buffer->end + 1);
}

gap_buffer *GbCopyBuffer(gap_buffer *buffer) {
  gap_buffer *bufferCopy = malloc(sizeof(gap_buffer));
  bufferCopy->cursor_start = buffer->cursor_start;
  bufferCopy->cursor_end = buffer->cursor_end;
  bufferCopy->end = buffer->end;
  bufferCopy->data = malloc(buffer->end + 2);
  strcpy(bufferCopy->data, buffer->data);
  bufferCopy->data[bufferCopy->end + 1] = '\0';
  return bufferCopy;
}

bool GbFlushBuffer(gap_buffer *buffer) {
  if (GbIsFlushed(buffer)) {
    return false;
  }
  memcpy(&buffer->data[buffer->cursor_start],
         &buffer->data[buffer->cursor_end + 1],
         buffer->end - buffer->cursor_end);
  buffer->data = realloc(buffer->data, -GbCursorSize(buffer) * sizeof(char));
  buffer->end -= GbCursorSize(buffer);
  buffer->data[buffer->end + 1] = '\0';
  buffer->cursor_start = -1;
  buffer->cursor_end = -1;
  return true;
}

// Doesn't work for resizing to smaller gap, code review
void GbResizeCursor(gap_buffer *buffer, long gap_size) {
  char *endData = malloc((buffer->end - buffer->cursor_end + 1));
  strcpy(endData, &buffer->data[buffer->cursor_end + 1]);
  long gapSizeDelta = gap_size - GbCursorSize(buffer);
  buffer->data = realloc(buffer->data, gapSizeDelta * sizeof(char));
  buffer->cursor_end += gapSizeDelta;
  buffer->end += gapSizeDelta;
  strcpy(&buffer->data[buffer->cursor_end + 1], endData);
  buffer->data[buffer->end + 1] = '\0';
}

void GbInsertCursor(gap_buffer *buffer, long insert_position, long gap_size) {
  if (!GbIsFlushed(buffer)) {
    GbFlushBuffer(buffer);
  }
  gap_buffer *bufferCopy = GbCopyBuffer(buffer);
  free(buffer->data);
  buffer->cursor_start = insert_position;
  buffer->cursor_end = insert_position + gap_size - 1;
  buffer->end += gap_size;
  buffer->data = malloc((buffer->end + 2) * sizeof(char));
  if (buffer->cursor_start > 0) {
    memcpy(buffer->data, bufferCopy->data, (insert_position) * sizeof(char));
  }
  if (buffer->cursor_end < buffer->end) {
    memcpy(&buffer->data[buffer->cursor_end + 1],
           &bufferCopy->data[insert_position],
           (bufferCopy->end - insert_position + 1) * sizeof(char));
  }
}

void GbPrintBufferDebug(gap_buffer *buffer) {
  printf((char *)"\nCursor Start: ");
  printf("%ld\n", buffer->cursor_start);
  printf((char *)"Cursor End: ");
  printf("%ld\n", buffer->cursor_end);
  printf((char *)"Cursor Length: ");
  printf("%ld\n", GbCursorSize(buffer));
  printf((char *)"Text Length: ");
  printf("%ld\n", GbTextSize(buffer));
  printf((char *)"NULL Termination: ");
  if (buffer->data[buffer->end + 1] == '\0') {
    printf((char *)"True\n");
  } else {
    printf((char *)"False\n");
  }

  if (!GbIsFlushed(buffer)) {
    for (int i = 0; i < buffer->cursor_start; i++) {
      char printChar = buffer->data[i];
      printf("%s", &printChar);
    }
    for (int i = buffer->cursor_start; i <= buffer->cursor_end; i++) {
      printf((char *)"~");
    }
    for (int i = buffer->cursor_end + 1; i <= buffer->end; i++) {
      char printChar = buffer->data[i];
      printf("%s", &printChar);
    }
    printf((char *)"\n");
    for (int i = 0; i < buffer->cursor_start; i++) {
      printf((char *)" ");
    }
    printf((char *)"C");
    for (int i = buffer->cursor_start + 1; i < buffer->cursor_end; i++) {
      printf((char *)"-");
    }
    printf((char *)"E\n");
  } else {
    for (int i = 0; i <= buffer->end; i++) {
      char printChar = buffer->data[i];
      printf("%s", &printChar);
    }
    printf((char *)"\n");
  }
}

bool GbInsertChar(gap_buffer *buffer, char input) {
  if (GbCursorSize(buffer) > 0) {
    buffer->data[buffer->cursor_start] = input;
    buffer->cursor_start++;
    return true;
  }
  return false;
}

bool GbMoveLeft(gap_buffer *buffer) {
  if (buffer->cursor_start > 0) {
    buffer->data[buffer->cursor_end] = buffer->data[buffer->cursor_start - 1];
    buffer->cursor_start--;
    buffer->cursor_end--;
    return true;
  }
  return false;
}

bool GbMoveRight(gap_buffer *buffer) {
  if (buffer->cursor_end < buffer->end) {
    buffer->data[buffer->cursor_start] = buffer->data[buffer->cursor_end + 1];
    buffer->cursor_start++;
    buffer->cursor_end++;
    return true;
  }
  return false;
}

bool GbDelete(gap_buffer *buffer) {
  if (buffer->cursor_end < buffer->end) {
    buffer->cursor_end++;
    return true;
  }
  return false;
}

bool GbBackspace(gap_buffer *buffer) {
  if (buffer->cursor_start > 0) {
    buffer->cursor_start--;
    return true;
  }
  return false;
}

char *GbConcatenate(gap_buffer *buffer) {
  if (!GbIsFlushed(buffer)) {

    char *concat = malloc((GbTextSize(buffer) + 1) * sizeof(char));
    memcpy(concat, buffer->data, buffer->cursor_start * sizeof(char));
    memcpy(&concat[buffer->cursor_start], &buffer->data[buffer->cursor_end + 1],
           (buffer->end - buffer->cursor_end) * sizeof(char));

    concat[GbTextSize(buffer)] = '\0';
    return concat;
  } else {
    char *concat = malloc((GbTextSize(buffer) + 1) * sizeof(char));
    strcpy(concat, buffer->data);
    return concat;
  }
}
