#include "gap_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pretty useless function but exists just in case
//
// It sets all indexes to -1 an creates a single char wide data arry that simply
// gets a null Termination
//
// This function will most likely break if then used with any sort of other
// functions so yeah, pretty useless
gap_buffer *GbInitEmptyBuffer() {
  gap_buffer *buffer = malloc(sizeof(gap_buffer));
  buffer->cursor_start = -1;
  buffer->cursor_end = -1;
  buffer->end = -1;
  buffer->data = malloc(sizeof(char));
  buffer->data[0] = '\0';
  return buffer;
}

// Inits a buffer to a passed size, the whole buffer is a cursor_start
//
// Should be self explanatory
gap_buffer *GbInitBuffer(long req_size) {
  gap_buffer *buffer = malloc(sizeof(gap_buffer));
  buffer->cursor_start = 0;
  buffer->end = req_size - 1;
  buffer->cursor_end = buffer->end;
  buffer->data = malloc((req_size + 1) * sizeof(char));
  buffer->data[req_size] = '\0';
  return buffer;
}

// Inits a flushed buffer using a passed string
//
// Again should be self explanatory
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

// Destroys things
void GbDestroy(gap_buffer *buffer) {
  free(buffer->data);
  free(buffer);
}

// Checks if a buffer is in its flushed state
//
// Flushed state meaning both cursor indexes are at -1
bool GbIsFlushed(gap_buffer *buffer) {
  if (buffer->cursor_start == -1 && buffer->cursor_end == -1) {
    return true;
  }
  return false;
}

// Returns the cursor width
long GbCursorSize(gap_buffer *buffer) {
  return (buffer->cursor_end - buffer->cursor_start + 1);
}

// Returns the stored text length
long GbTextSize(gap_buffer *buffer) {
  if (!GbIsFlushed(buffer)) {
    return (buffer->end - buffer->cursor_end + buffer->cursor_start );
  }
  return (buffer->end + 1);
}

// Copies buffer
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

// Flushes buffer by first shifting buffer end data to concatenate it together
// then realloc to trim the buffer
//
//  TODO : Confirm if memcpy and realloc implementation is correct and safe
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

// Resizes cursor to given size
//
// Confrmed working to gap_size > current_gap_size
//  TODO : Fix for gap_size < current_gap_size, currently broken
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

// Inserts cursor to a given position in a flushed buffer
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

// Prints alot of stuff, doesn't really need messing with unless its to add more
// debug info
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

// Insert char at cursor_start
bool GbInsertChar(gap_buffer *buffer, char input) {
  if (GbCursorSize(buffer) > 0) {
    buffer->data[buffer->cursor_start] = input;
    buffer->cursor_start++;
    return true;
  }
  return false;
}

// Move cursor left by shifting single char from cursor_start - 1 to cursor_end
// then shifting cursor indexes by -1
bool GbMoveLeft(gap_buffer *buffer) {
  if (buffer->cursor_start > 0) {
    buffer->data[buffer->cursor_end] = buffer->data[buffer->cursor_start - 1];
    buffer->cursor_start--;
    buffer->cursor_end--;
    return true;
  }
  return false;
}

// Move cursor right by shifting single char from cursor_end + 1 to cursor_start
// then shifting cursor indexes by +1
bool GbMoveRight(gap_buffer *buffer) {
  if (buffer->cursor_end < buffer->end) {
    buffer->data[buffer->cursor_start] = buffer->data[buffer->cursor_end + 1];
    buffer->cursor_start++;
    buffer->cursor_end++;
    return true;
  }
  return false;
}

// Deletes char right of cursor_end by shifting cursor_end by +1
bool GbDelete(gap_buffer *buffer) {
  if (buffer->cursor_end < buffer->end) {
    buffer->cursor_end++;
    return true;
  }
  return false;
}

// Delets char left of cursor_start by moving cursor_start by -1
bool GbBackspace(gap_buffer *buffer) {
  if (buffer->cursor_start > 0) {
    buffer->cursor_start--;
    return true;
  }
  return false;
}

// Returns concatenated string of buffer without flushing the buffer in the case
// of an open buffer to avoid large memory operations
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
