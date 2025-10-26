#include <bits/pthreadtypes.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct linked_buffer_t {
  char* data;
  int current;
  int size;
  int height;
  struct linked_buffer_t** sub_buffers;
  int* buff_index;
  int nb;
}lBuffer;

void lBAppend(lBuffer* lb,char c){
  if(c=='\n'){
    lb->height++;
  }
  if(lb->current==lb->size-1){
    lb->data = realloc(lb->data,2*lb->size);
    memset(lb->data+lb->size,'\0',lb->size);
    lb->size*=2;
  }
  lb->data[lb->current] =c;
  lb->current++;
}

void lBDelete(lBuffer* lb,int pos){
  if(lb->data[pos]=='\n'){
    lb->height--;
  }
  strcpy(lb->data+pos+1,lb->data+pos);
  lb->data[lb->current]='\0';
  lb->current--;
}

int lBTotalSize(lBuffer* lb){
  int size=lb->current;
  for(int i=0;i<lb->nb;++i){
    if(lb->sub_buffers[i]!=NULL){
      size+=lBTotalSize(lb->sub_buffers[i]);
    }
  }
  return size;
}
int lBTotalHeight(lBuffer* lb){
  int height=lb->current;
  for(int i=0;i<lb->nb;++i){
    if(lb->sub_buffers[i]!=NULL){
      height+=lBTotalSize(lb->sub_buffers[i]);
    }
  }
  return height;
}

void lBInsertBuffer(lBuffer* lb,lBuffer* b,int pos){
  lb->nb++;
  lb->sub_buffers=realloc(lb->sub_buffers,lb->nb*sizeof(void*));
  lb->sub_buffers[lb->nb-1]=b;
  lb->buff_index=realloc(lb->buff_index,lb->nb*sizeof(int));
  lb->buff_index[lb->nb-1]=pos;
  lb->data[pos]='\0';

}

void lBAbsInsert(lBuffer* lb,int pos,char ch){
  if(pos==lBTotalSize(lb)){
    lBAppend(lb,ch);
  }
  int c =0;
  int tot =0;
  for(int i=0;i<lb->nb;i++){
    c+=strlen(lb->data+c+i);
    tot+=strlen(lb->data+c+i);
  }
}

void DrawLBasText(lBuffer* lb,int *x,int *y,int size,Color c){
  DrawText(lb->data,*x,*y,size,c);
  *x+=MeasureText(lb->data,size);
  *y+=size*lb->height;
  for(int i=0;i<lb->nb;++i){
    if(lb->sub_buffers[i]!=NULL){
      DrawLBasText(lb->sub_buffers[i],x,y,size,c);
      DrawText(lb->data+lb->buff_index[i],*x,*y,size,c);
    }
  }
}

typedef struct gap_buffer_t {
  unsigned int start;
  unsigned int end;
  unsigned int cursor_start; // relative to buffer start
  unsigned int cursor_end; // Seems redundant
  char *data;
} gap_buffer;

void GapBufferInit(gap_buffer *buffer, unsigned int req_size) {
  buffer->data = malloc((req_size+1) * sizeof(char));
  buffer->data[req_size]='\0';
  buffer->start = 0;
  buffer->end = req_size - 1;
  buffer->cursor_start = buffer->start;
  buffer->cursor_end = buffer->end;
}

void GapBufferInitDebug(gap_buffer *buffer, unsigned int req_size) {
  GapBufferInit(buffer, req_size);

  for (int i = 0; i < req_size; i++) {
    buffer->data[i] = '-';
  }
}

void GapBufferDestroy(gap_buffer *buffer) { free(buffer->data); }

void GapBufferInsert(gap_buffer *buffer, char insert) {
  if(buffer->cursor_start==(buffer->end-buffer->start)){
    buffer->data = realloc(buffer->data,1+(buffer->end-buffer->start)*2);
    buffer->data[(buffer->end-buffer->start)*2]='\0';
    buffer->end=buffer->start+1+(buffer->end-buffer->start)*1.5;
    buffer->cursor_end=buffer->start+1+(buffer->end-buffer->start)*1.5;
  }
  buffer->data[buffer->cursor_start] = insert;
  buffer->cursor_start++;
}

void GapBufferPrintBufferDebug(gap_buffer *buffer) {
  for (int i = buffer->start; i <= buffer->end; i++) {
    char printChar = buffer->data[i];
    printf("%s", &printChar);
  }
  printf("\n");
  for (int i = buffer->start; i < buffer->cursor_start; i++) {
    printf((char *)" ");
  }
  printf((char *)"C");
  for (int i = buffer->cursor_start + 1; i < buffer->cursor_end; i++) {
    printf((char *)" ");
  }
  printf((char *)"E");
  for (int i = buffer->cursor_end + 1; i <= buffer->end; i++) {
    printf((char *)" ");
  }
  printf("\n");
}

bool GapBufferMoveLeft(gap_buffer *buffer, unsigned int amount) {
  if (amount <= buffer->cursor_start) {
    for (int i = 0; i < amount; i++) {
      buffer->data[buffer->cursor_end] = buffer->data[buffer->cursor_start - 1];
      buffer->cursor_start--;
      buffer->cursor_end--;
    }
    return true;
  }
  return false;
}

bool GapBufferMoveRight(gap_buffer *buffer, unsigned int amount) {
  if (amount <= buffer->end - buffer->cursor_end) {
    for (int i = 0; i < amount; i++) {
      buffer->data[buffer->cursor_start] = buffer->data[buffer->cursor_end + 1];
      buffer->cursor_start++;
      buffer->cursor_end++;
    }
    return true;
  }
  return false;
}

int main(int argc, char *argv[]) {
  if(argc>1){
    //TODO handle the case where a file name is passed as an arg
  }
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(720, 480, (char *)"SpiderType");
  while (!IsWindowReady()) {
  };
  SetWindowMaxSize(3480, 2160);
  SetWindowMinSize(200, 100);
  SetTargetFPS(60);
  printf("Window ready\n");
  gap_buffer testBuff;
  int cursorLocation[2]={0,0};
  char c ='\0';
  GapBufferInitDebug(&testBuff, 10);
  GapBufferPrintBufferDebug(&testBuff);
  GapBufferInsert(&testBuff, 'A');
  GapBufferPrintBufferDebug(&testBuff);
  GapBufferMoveLeft(&testBuff, 1);
  GapBufferPrintBufferDebug(&testBuff);
  while (!WindowShouldClose()) {
    c = GetCharPressed();
    while(c!=0){
      cursorLocation[0]+=1;
      c = GetCharPressed();
    }
    if(IsKeyPressed(KEY_LEFT)){
      cursorLocation[0]+=-1+(cursorLocation[0]==0);
    }else if(IsKeyPressed(KEY_RIGHT)){
    }

    BeginDrawing();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), BLACK);
    DrawText("Test",0,0,10,ORANGE);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
