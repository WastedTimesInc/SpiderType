#pragma once
#include "gap_buffer.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct text_buffer_t {
  gap_buffer **buffer_lines; // Array of gap_buffer pointers
  char **concat_lines;       // Array of concatenated gap_buffer pointers
  bool *is_modified;         // Array of boolean flag modified, probably useless
  long current_line; // Current line index, this stores the array index, ie
                     // starts at 0
  long num_lines; // Number of lines in the text buffer, this is NOT an index,
                  // start at 1
  long last_cursor_pos; // Last cursor position, used for up down movement to
                        // preserve position
  long cursor_resize;
} text_buffer;

// Init a text buffer with num_lines and req_size determines the default
// gap_buffer size
text_buffer *TbInitBuffer(long num_lines, long req_size);
// Insert a line to the right, ie below, insert_idx
bool TbInsertLineAt(text_buffer *buffer, long insert_idx, long req_size);

bool TbInsertLineAtWithString(text_buffer *buffer, long insert_idx,
                              long req_size, char *input);
// Remove the line at remove_idx
bool TbRemoveLineAt(text_buffer *buffer, long remove_idx);

// Merges merge_idx and merge_idx+1 into merge_idx
bool TbMergeLines(text_buffer *buffer, long merge_idx);

// Returns current_line which is an index
long TbLinePosition(text_buffer *buffer);
// Returns the cursor index of the current_line
long TbCursorPosition(text_buffer *buffer);
// Returns true if current_line is at the end of the buffer
bool TbEndOfBuffer(text_buffer *buffer);
// Returns ture if the current_line is at the start of the buffer
bool TbStartOfBuffer(text_buffer *buffer);

// Safe move left that jumps line if the cursor is at the start of current_line
void TbMoveLeft(text_buffer *buffer);
// Safe move right that jumps line if the cursor is at the end of current_line
void TbMoveRight(text_buffer *buffer);
// Safe backspace the deletes the current line if the cursor is at the start of
// the line, currently doesn't support merging two lines should there be content
// on current_line and backspace is called with the cursor at the start of the
// line
void TbBackspace(text_buffer *buffer);
// Safe delete, similarly to backspace, this is a non complete implementation
void TbDelete(text_buffer *buffer);
// Move up using last_cursor_pos to preserve cursor position when moving up past
// lines that aren't long enough
void TbMoveUp(text_buffer *buffer);
// Move down using last_cursor_pos to preserve cursor position when moving down
// past lines that aren't long enough
void TbMoveDown(text_buffer *buffer);

void TbEnter(text_buffer *buffer);

// Safe char insert
bool TbInsertChar(text_buffer *buffer, char insert);

void TbDestroy(text_buffer *buffer);

#ifndef TEXT_BUFFER
#define TEXT_BUFFER

text_buffer *TbInitBuffer(long num_lines, long req_size) {
  text_buffer *buffer = malloc(sizeof(text_buffer));
  buffer->num_lines = num_lines;
  buffer->current_line = num_lines - 1;
  buffer->buffer_lines = malloc((buffer->num_lines) * sizeof(gap_buffer *));
  buffer->concat_lines = malloc((buffer->num_lines) * sizeof(char *));
  buffer->is_modified = malloc((buffer->num_lines) * sizeof(bool));
  for (int i = 0; i < buffer->num_lines; i++) {
    buffer->buffer_lines[i] = GbInitBuffer(req_size);
    buffer->concat_lines[i] = GbConcatenate(buffer->buffer_lines[i]);
    buffer->is_modified[i] = false;
  }
  buffer->cursor_resize = req_size;
  return buffer;
}

bool TbInsertLineAt(text_buffer *buffer, long insert_idx, long req_size) {
  if (insert_idx < buffer->num_lines) {
    GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
    buffer->buffer_lines = realloc(
        buffer->buffer_lines, (buffer->num_lines + 1) * sizeof(gap_buffer *));
    buffer->concat_lines =
        realloc(buffer->concat_lines, (buffer->num_lines + 1) * sizeof(char *));
    buffer->is_modified =
        realloc(buffer->is_modified, (buffer->num_lines + 1) * sizeof(bool));
    memcpy(&buffer->buffer_lines[insert_idx + 2],
           &buffer->buffer_lines[insert_idx + 1],
           (buffer->num_lines - insert_idx - 1) * sizeof(gap_buffer *));
    memcpy(&buffer->concat_lines[insert_idx + 2],
           &buffer->concat_lines[insert_idx + 1],
           (buffer->num_lines - insert_idx - 1) * sizeof(char *));
    memcpy(&buffer->is_modified[insert_idx + 2],
           &buffer->is_modified[insert_idx + 1],
           (buffer->num_lines - insert_idx - 1) * sizeof(bool));
    buffer->buffer_lines[insert_idx + 1] = GbInitBuffer(req_size);
    buffer->concat_lines[insert_idx + 1] =
        GbConcatenate(buffer->buffer_lines[insert_idx + 1]);
    buffer->is_modified[insert_idx + 1] = false;
    buffer->num_lines++;
    buffer->current_line++;
    return true;
  }
  return false;
}

bool TbInsertLineAtWithString(text_buffer *buffer, long insert_idx,
                              long req_size, char *input) {
  if (insert_idx < buffer->num_lines) {
    GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
    buffer->buffer_lines = realloc(
        buffer->buffer_lines, (buffer->num_lines + 1) * sizeof(gap_buffer *));
    buffer->concat_lines =
        realloc(buffer->concat_lines, (buffer->num_lines + 1) * sizeof(char *));
    buffer->is_modified =
        realloc(buffer->is_modified, (buffer->num_lines + 1) * sizeof(bool));
    memcpy(&buffer->buffer_lines[insert_idx + 2],
           &buffer->buffer_lines[insert_idx + 1],
           (buffer->num_lines - insert_idx - 1) * sizeof(gap_buffer *));
    memcpy(&buffer->concat_lines[insert_idx + 2],
           &buffer->concat_lines[insert_idx + 1],
           (buffer->num_lines - insert_idx - 1) * sizeof(char *));
    memcpy(&buffer->is_modified[insert_idx + 2],
           &buffer->is_modified[insert_idx + 1],
           (buffer->num_lines - insert_idx - 1) * sizeof(bool));
    buffer->buffer_lines[insert_idx + 1] = GbInitFlushBufferFromString(input);
    GbInsertCursor(buffer->buffer_lines[buffer->current_line],
                   GbTextSize(buffer->buffer_lines[buffer->current_line]),
                   buffer->cursor_resize);
    buffer->concat_lines[insert_idx + 1] =
        GbConcatenate(buffer->buffer_lines[insert_idx + 1]);
    buffer->is_modified[insert_idx + 1] = false;
    buffer->num_lines++;
    buffer->current_line++;
    return true;
  }
  return false;
}

bool TbRemoveLineAt(text_buffer *buffer, long remove_idx) {
  if (remove_idx < buffer->num_lines) {
    if (buffer->current_line == remove_idx) {
      GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
      buffer->current_line--;
      GbInsertCursor(buffer->buffer_lines[buffer->current_line],
                     GbTextSize(buffer->buffer_lines[buffer->current_line]),
                     buffer->cursor_resize);
    }
    memcpy(&buffer->buffer_lines[remove_idx],
           &buffer->buffer_lines[remove_idx + 1],
           (buffer->num_lines - remove_idx - 1) * sizeof(gap_buffer *));
    memcpy(&buffer->concat_lines[remove_idx],
           &buffer->concat_lines[remove_idx + 1],
           (buffer->num_lines - remove_idx - 1) * sizeof(char *));
    memcpy(&buffer->is_modified[remove_idx],
           &buffer->is_modified[remove_idx + 1],
           (buffer->num_lines - remove_idx - 1) * sizeof(bool));
    buffer->buffer_lines = realloc(
        buffer->buffer_lines, (buffer->num_lines - 1) * sizeof(gap_buffer *));
    buffer->concat_lines =
        realloc(buffer->concat_lines, (buffer->num_lines - 1) * sizeof(char *));
    buffer->is_modified =
        realloc(buffer->is_modified, (buffer->num_lines - 1) * sizeof(bool));
    buffer->num_lines--;
    return true;
  }
  return false;
}

bool TbMergeLines(text_buffer *buffer, long merge_idx) {
  if (merge_idx < buffer->num_lines - 1) {
    char *lineOne = GbConcatenate(buffer->buffer_lines[merge_idx]);
    char *lineTwo = GbConcatenate(buffer->buffer_lines[merge_idx + 1]);
    lineOne = realloc(lineOne,
                      (strlen(lineOne) + strlen(lineTwo) + 1) * sizeof(char));
    strcpy(&lineOne[strlen(lineOne)], lineTwo);
    GbOverwriteWithString(buffer->buffer_lines[merge_idx], lineOne,
                          buffer->cursor_resize);
    TbRemoveLineAt(buffer, merge_idx + 1);
    buffer->concat_lines[merge_idx] =
        GbConcatenate(buffer->buffer_lines[merge_idx]);
  }
  return false;
}

long TbLinePosition(text_buffer *buffer) { return buffer->current_line; }

long TbCursorPosition(text_buffer *buffer) {
  return GbCursorPosition(buffer->buffer_lines[buffer->current_line]);
}

bool TbStartOfBuffer(text_buffer *buffer) {
  if (buffer->current_line == 0) {
    return true;
  }
  return false;
}

bool TbEndOfBuffer(text_buffer *buffer) {
  if (buffer->current_line == buffer->num_lines - 1) {
    return true;
  }
  return false;
}

void TbMoveLeft(text_buffer *buffer) {
  if (GbCursorStartOfLine(buffer->buffer_lines[buffer->current_line]) &&
      !TbStartOfBuffer(buffer)) {
    GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
    buffer->current_line--;
    GbInsertCursor(buffer->buffer_lines[buffer->current_line],
                   GbTextSize(buffer->buffer_lines[buffer->current_line]),
                   buffer->cursor_resize);
  } else {
    GbMoveLeft(buffer->buffer_lines[buffer->current_line]);
  }
  buffer->last_cursor_pos =
      GbCursorPosition(buffer->buffer_lines[buffer->current_line]);
}

void TbMoveRight(text_buffer *buffer) {
  if (GbCursorEndOfLine(buffer->buffer_lines[buffer->current_line])) {
    if (buffer->current_line != buffer->num_lines - 1) {
      GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
      buffer->current_line++;
      GbInsertCursor(buffer->buffer_lines[buffer->current_line], 0,
                     buffer->cursor_resize);
    }
  } else {
    GbMoveRight(buffer->buffer_lines[buffer->current_line]);
  }
  buffer->last_cursor_pos =
      GbCursorPosition(buffer->buffer_lines[buffer->current_line]);
}

// TODO fix backspace to append string to line above when backapce from non
// empty curosr 0 idx
void TbBackspace(text_buffer *buffer) {
  if (GbCursorStartOfLine(buffer->buffer_lines[buffer->current_line]) &&
      !TbStartOfBuffer(buffer)) {
    if (GbTextSize(buffer->buffer_lines[buffer->current_line]) == 0) {
      TbRemoveLineAt(buffer, buffer->current_line);
    } else {
      TbMergeLines(buffer, buffer->current_line - 1);
    }
  } else {
    GbBackspace(buffer->buffer_lines[buffer->current_line]);
    free(buffer->concat_lines[buffer->current_line]);
    buffer->concat_lines[buffer->current_line] =
        GbConcatenate(buffer->buffer_lines[buffer->current_line]);
  }
  buffer->last_cursor_pos =
      GbCursorPosition(buffer->buffer_lines[buffer->current_line]);
}

void TbDelete(text_buffer *buffer) {
  if (GbCursorEndOfLine(buffer->buffer_lines[buffer->current_line])) {
    TbBackspace(buffer);
  } else {
    GbDelete(buffer->buffer_lines[buffer->current_line]);
    free(buffer->concat_lines[buffer->current_line]);
    buffer->concat_lines[buffer->current_line] =
        GbConcatenate(buffer->buffer_lines[buffer->current_line]);
  }
}

void TbMoveUp(text_buffer *buffer) {
  if (!TbStartOfBuffer(buffer)) {
    GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
    buffer->current_line--;
    if (buffer->last_cursor_pos <=
        GbTextSize(buffer->buffer_lines[buffer->current_line])) {
      GbInsertCursor(buffer->buffer_lines[buffer->current_line],
                     buffer->last_cursor_pos, buffer->cursor_resize);
    } else {
      GbInsertCursor(buffer->buffer_lines[buffer->current_line],
                     GbTextSize(buffer->buffer_lines[buffer->current_line]),
                     buffer->cursor_resize);
    }
  }
}

void TbMoveDown(text_buffer *buffer) {
  if (!TbEndOfBuffer(buffer)) {
    GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
    buffer->current_line++;
    if (buffer->last_cursor_pos <=
        GbTextSize(buffer->buffer_lines[buffer->current_line])) {
      GbInsertCursor(buffer->buffer_lines[buffer->current_line],
                     buffer->last_cursor_pos, buffer->cursor_resize);
    } else {
      GbInsertCursor(buffer->buffer_lines[buffer->current_line],
                     GbTextSize(buffer->buffer_lines[buffer->current_line]),
                     buffer->cursor_resize);
    }
  }
}

// TODO Fix line splitting when pressing enter inside a line
void TbEnter(text_buffer *buffer) {
  if (GbCursorEndOfLine(buffer->buffer_lines[buffer->current_line])) {
    TbInsertLineAt(buffer, buffer->current_line, buffer->cursor_resize);
  } else {
    char *leftText =
        GbConcatenateLeft(buffer->buffer_lines[buffer->current_line]);
    char *rightText =
        GbConcatenateRight(buffer->buffer_lines[buffer->current_line]);
    GbOverwriteWithString(buffer->buffer_lines[buffer->current_line], leftText,
                          buffer->cursor_resize);
    buffer->concat_lines[buffer->current_line] =
        GbConcatenate(buffer->buffer_lines[buffer->current_line]);
    TbInsertLineAtWithString(buffer, buffer->current_line,
                             buffer->cursor_resize, rightText);
    GbInsertCursor(buffer->buffer_lines[buffer->current_line], 0,
                   buffer->cursor_resize);
  }
}

bool TbInsertChar(text_buffer *buffer, char insert) {
  bool success =
      GbInsertChar(buffer->buffer_lines[buffer->current_line], insert);
  if (!success) {
    GbResizeCursor(buffer->buffer_lines[buffer->current_line],
                   buffer->cursor_resize);
    success = GbInsertChar(buffer->buffer_lines[buffer->current_line], insert);
  }
  if (success) {
    buffer->concat_lines[buffer->current_line] =
        GbConcatenate(buffer->buffer_lines[buffer->current_line]);
  }
  return success;
}

void TbDestroy(text_buffer *buffer) {
  for (int i = 0; i < buffer->num_lines; i++) {
    GbDestroy(buffer->buffer_lines[i]);
    free(buffer->buffer_lines[i]);
    free(buffer->concat_lines[i]);
  }
}
#endif
