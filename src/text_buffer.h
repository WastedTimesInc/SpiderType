#include "gap_buffer.h"
#include <stdlib.h>
#include <string.h>

typedef struct text_buffer_t {
  gap_buffer **buffer_lines;
  char **concat_lines;
  bool *is_modified;
  long current_line;
  long num_lines;
} text_buffer;

void TbInitBuffer(text_buffer *buffer, long num_lines, long req_size);
bool TbInsertLineAt(text_buffer *buffer, long insert_idx, long req_size);

long TbLinePosition(text_buffer *buffer);
long TbCursorPosition(text_buffer *buffer);

void TbMoveLeft(text_buffer *buffer);

#ifndef TEXT_BUFFER
#define TEXT_BUFFER

void TbInitBuffer(text_buffer *buffer, long num_lines, long req_size) {
  buffer->num_lines = num_lines;
  buffer->current_line = num_lines - 1;
  buffer->buffer_lines = malloc((buffer->num_lines) * sizeof(gap_buffer *));
  buffer->concat_lines = malloc((buffer->num_lines) * sizeof(char *));
  buffer->is_modified = malloc((buffer->num_lines) * sizeof(bool));
  for (int i = 0; i < buffer->num_lines; i++) {
    GbInitBufferByPointer(buffer->buffer_lines[i], req_size);
    buffer->concat_lines[i] = GbConcatenate(buffer->buffer_lines[i]);
    buffer->is_modified[i] = false;
  }
}

bool TbInsertLineAt(text_buffer *buffer, long insert_idx, long req_size) {
  if (insert_idx < buffer->num_lines) {
    GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
    buffer->buffer_lines = realloc(
        buffer->buffer_lines, (buffer->num_lines + 1) * sizeof(gap_buffer *));
    buffer->concat_lines = realloc(
        buffer->concat_lines, (buffer->num_lines + 1) * sizeof(gap_buffer));
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
    return true;
  }
  return false;
}

long TbLinePosition(text_buffer *buffer) { return buffer->current_line; }

long TbCursorPosition(text_buffer *buffer) {
  return GbCursorPosition(buffer->buffer_lines[buffer->current_line]);
}

void TbMoveLeft(text_buffer *buffer) {
  if (GbCursorPosition(buffer->buffer_lines[buffer->current_line]) == 0) {
    GbFlushBuffer(buffer->buffer_lines[buffer->current_line]);
  }
}

#endif
