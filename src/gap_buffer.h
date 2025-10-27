/*
 *Gap Buffer
 *
 *Data Structure
 *The gap_buffer consists of three indexes:
 *  cursor_start
 *  cursor_end
 *  end
 *and data which is a NULL terminated char* array
 *
 *Here is an example of the structure where A,B,C correspond to the three
 *indexes respectively
 *
 *arr idx : 0 1 2 3 4 5 6 7 8
 *data    : H E L         L O
 *idx     :       A     B   C
 *
 *A gap_buffer can be in one of two states:
 *  Open - a cursor gap is present in the buffer
 *  Flushed -  a cursor gap is NOT present in the buffer
 *
 *A flushed buffer is identified as having both cursor indexes set to -1
 *
 *All functions below have a short description above them and a more in depth
 *description in gap_buffer.c
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct gap_buffer_t {
  long cursor_start;
  long cursor_end;
  long end;
  char *data;
} gap_buffer;

// Initialize an empty buffer
gap_buffer *GbInitEmptyBuffer();
// Initialize an empty open buffer with a given cursor width
gap_buffer *GbInitBuffer(long req_size);
// Initialize a flushed buffer from a string passed as input
gap_buffer *GbInitFlushBufferFromString(char *input);

// Destroy the buffer passed
void GbDestroy(gap_buffer *buffer);

// Checks if a buffer is flushed
bool GbIsFlushed(gap_buffer *buffer);
// Returns the cursor width in an open buffer
long GbCursorSize(gap_buffer *buffer);
// Returns the text length stored in an open or flushed buffer
long GbTextSize(gap_buffer *buffer);

// Returns a pointer to a copied buffer
gap_buffer *GbCopyBuffer(gap_buffer *buffer);

// Flushes the buffer passed
bool GbFlushBuffer(gap_buffer *buffer);
// Resizes the cursor on an open buffer to gap-size
void GbResizeCursor(gap_buffer *buffer, long gap_size);
// Inserts the cursor on a flushed buffer to the left of insert_position
void GbInsertCursor(gap_buffer *buffer, long insert_position, long gap_size);

// Prints a host of debug information about a buffer to help hunt down issues
void GbPrintBufferDebug(gap_buffer *buffer);

// Inserts a char at start_cursor
bool GbInsertChar(gap_buffer *buffer, char input);
// Moves the cursor one to the left
bool GbMoveLeft(gap_buffer *buffer);
// Moves the cursor one to the right
bool GbMoveRight(gap_buffer *buffer);
// Deletes the character to the right of the cursor
bool GbDelete(gap_buffer *buffer);
// Deletes the character to the left of the cursor
bool GbBackspace(gap_buffer *buffer);

// Returns the concatenated string of a buffer
char *GbConcatenate(gap_buffer *buffer);
