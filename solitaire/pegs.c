/**
 * @file pegs.c
 * @author YAN SUN
 * @version 1.0
 *
 * @section DESCRIPTION
 * This program implements Peg solitaire game written in C.
 * This program implements both BFS and DFS.
 * This program makes use of the SDL to provide a graphical interface for this application.
 */
#include <stdio.h>
#include <stdlib.h>
#include "neillsdl2.h"
#define SIZE 5
#define SUCCESS 1
#define FAIL 0
#define GO_UP 1
#define GO_DOWN 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define PEG 'O'
#define SPACE '.'
#define UNFILLED 'i'
#define WHITE 255
#define BLACK 0
#define GREY 192
#define BFS 0
#define DFS 1
#define ON 1
#define OFF 0
#define MILLISECONDDELAY 800

struct node{
  char board[SIZE][SIZE];
  int step;
  int flag;
  struct node *next;
  struct node *previous;
};
typedef struct node Node;

struct queue{
  Node *front;
  Node *back;
};
typedef struct queue Queue;

int initialise(char board[SIZE][SIZE]);
int readFile(FILE *file, char *argv[], char board[SIZE][SIZE]);
Node *moveDFS(char board[SIZE][SIZE], Node *start);
int moveForward(char board[SIZE][SIZE], int direction, int x, int y);
int moveBack(char board[SIZE][SIZE], int direction, int x, int y);
int checkLeft(char board[SIZE][SIZE], int x, int y);
int checkRight(char board[SIZE][SIZE], int x, int y);
int checkUp(char board[SIZE][SIZE], int x, int y);
int checkDown(char board[SIZE][SIZE], int x, int y);
int countPegs(char board[SIZE][SIZE]);
int printBoard(char board[SIZE][SIZE]);
int printSteps(Node *current);
Node *AllocateNode(char board[SIZE][SIZE]);
Node *storeBoard(char board[SIZE][SIZE], Node *current);
Node *reverseList(Node *list);
int drawMove(Node* start);
int drawBoard(char board[SIZE][SIZE], SDL_Simplewin sw);
Node *moveBFS(Queue *q);
Node *removeNode(Queue *q);
int insertNode(Queue *q, Node *p);
int initQueue(Queue *q);
int empty(Queue *q);
int printQueue(Queue *q);
double computeID(char board[SIZE][SIZE]);
int checkRepeat(Queue q, Node *p);
int versionSelect(int *sdl, int *mode);
int copyBoard(char toBoard[SIZE][SIZE],char fromBoard[SIZE][SIZE]);
int checkWin(char board[SIZE][SIZE]);
Node *storeParents(Node *nextNode, Node *p);

int main(int argc, char *argv[]){
  int sdl, mode;
  Queue q;
  Node *start, *current;
  char board[SIZE][SIZE];
  FILE *file;
  /* Initialisations */
  file = NULL;
  sdl = OFF;
  mode = BFS;
  initialise(board);
  initQueue(&q);
  readFile(file, argv, board);
  start = AllocateNode(board);
  insertNode(&q,start);
  /* Select version */
  versionSelect(&sdl, &mode);
  printf("Computing...\n");
  /* Select BFS or DFS base on the selection above*/
  if(mode == BFS){
    current = moveBFS(&q);
  }
  else{
    current = moveDFS(board,start);
  }
  /* Check whether solution exists*/
  if(current->flag == SUCCESS){
     printf("Solution found!\n");
     current = reverseList(current);
  }
  else{
    printf("No solution found!\n");
    return 0;
  }
  /* Show the correct solution in command line or in SDL*/
  if(sdl == ON){
    drawMove(current);
  }
  else{
    printSteps(current);
  }
  return 1;
}
/**
 * Allow users to input a number to select the version.
 * Terminate if the input is invalid.
 * 
 * @param sdl Switch of SDL.
 * @param mode Mode selection, BFS or DFS.
 * @return 1 on success,0 on failure.
 */
int versionSelect(int *sdl, int *mode){
  int version;
  printf("Please select the version : \n\n");
  printf("1. Basic version(BFS + Command line) \n");
  printf("2. SDL version(BFS + SDL) \n");
  printf("3. Extension(DFS + SDL) \n\n");
  printf("Please enter a number : ");
  scanf("%d",&version);

  if(version == 1){*mode = BFS;*sdl = OFF;}
  else if(version == 2){*mode = BFS;*sdl = ON;}
  else if(version == 3){*mode = DFS;*sdl = ON;}
  else{
    printf("Invalid input!\n");
    return 0;
  }
  return 1;
}
/**
 * Read the board from the input file.
 * Exit when the file does not exist.
 * 
 * @param file The file stream.
 * @param argv The file name gets from the command line.
 * @param board The initialised board.
 * @return 1 on success.
 */
int readFile(FILE *file, char *argv[], char board[SIZE][SIZE]){
  int i, j;
  file = fopen(argv[1], "r");
  /* fopen returns NULL pointer on failure */
  if (file == NULL){
    printf("Could not open file. \n");
    exit(2);
  }
  else {
    printf("File (%s) opened. \n", argv[1]);
    do{
      for(j = 0; j < SIZE; j++){
	for(i = 0; i < SIZE; i++){
	  board[j][i] = fgetc(file);
	}
	fgetc(file);
      }
    }while(fgetc(file) != EOF);
    /* Closing file */
    fclose(file);
  }
  return 1;
}
/**
 * Check the board whether it is a final winning solution.
 * 
 * @param board The board.
 * @return SUCCESS on success, FAIL on failure.
 */
int checkWin(char board[SIZE][SIZE]){
  if(countPegs(board) == 1 && board[SIZE/2][SIZE/2] == PEG){
    return SUCCESS;
  }
  return FAIL;
}
/**
 * Check the board in the node whether it already exists in
 * the queue or not.
 * 
 * @param q The Queue.
 * @param p The Node to check with the queue.
 * @return 1 on success. 0 on failure.
 */
int checkRepeat(Queue q, Node *p){
  double queueID,nodeID;
  Node *temp;
  char (*tempBoard)[SIZE];
  temp = q.front->next;
  nodeID = computeID(p->board);
  while(temp != NULL){
    tempBoard = temp->board;
    queueID = computeID(tempBoard);
    if(queueID == nodeID){
      return 1;
    }
    temp = temp->next;
  }
  return 0;
}
/**
 * Compute an unique id for the board.
 * 
 * @param board The board.
 * @return id Return the computed id.
 */
double computeID(char board[SIZE][SIZE]){
  int i, j;
  double id;
  id = 0;
  for(j = 0; j < SIZE; j++){
    for(i = 0; i < SIZE; i++){
      if(board[j][i] == PEG){
	id++;
      }
      id = id * 10;
    }
  }
  return id;
}
/**
 * Allocate the new node the parent node.
 * count the steps.
 * 
 * @param nextNode The new node.
 * @param p The parent node.
 * @return nextNode Return the new node.
 */
Node *storeParents(Node *nextNode, Node *p){
  nextNode->previous = p;
  nextNode->step = p->step + 1;
  return nextNode;
}
/**
 * Compute the correct move by using BFS.
 * It is achieved by using the queue.
 * Remove a node from the queue, and computes all possible next moves.
 * Insert all the moves in the queue.
 * Loop until find a solution.
 * @param q The queue.
 * @return result Return the final node with the flag.
 */
Node *moveBFS(Queue *q){
  int i, j;
  Node *result;
  result = AllocateNode(q->front->board);
  while(empty(q) != 1){
    Node *p, *nextNode;
    p = removeNode(q);
    if(checkWin(p->board) == SUCCESS){
      result = p;
      result->flag = SUCCESS;
      return result;
    }
    for(j = 0; j < SIZE; j++){
      for(i = 0; i < SIZE; i++){
	if(p->board[j][i] == PEG){
	  if(checkLeft(p->board, i, j) == GO_LEFT){
	    moveForward(p->board,GO_LEFT,i,j);
	    nextNode = AllocateNode(p->board);
	    storeParents(nextNode,p);
	    insertNode(q, nextNode);
	    moveBack(p->board,GO_LEFT,i,j);
	  }
	  if(checkRight(p->board, i, j) == GO_RIGHT){
	    moveForward(p->board,GO_RIGHT,i,j);
	    nextNode = AllocateNode(p->board);
	    storeParents(nextNode,p);
	    insertNode(q, nextNode);
	    moveBack(p->board,GO_RIGHT,i,j);
	  }
	  if(checkUp(p->board, i, j) == GO_UP){
	    moveForward(p->board,GO_UP,i,j);
	    nextNode = AllocateNode(p->board);
	    storeParents(nextNode,p);
	    insertNode(q, nextNode);
	    moveBack(p->board,GO_UP,i,j);
	  }
	  if(checkDown(p->board, i, j) == GO_DOWN){
	    moveForward(p->board,GO_DOWN,i,j);
	    nextNode = AllocateNode(p->board);
	    storeParents(nextNode,p);
	    insertNode(q, nextNode);
	    moveBack(p->board,GO_DOWN,i,j);
	  }
	}
      }
    }
  }
  result->flag = FAIL;
  return result;
}
/**
 * Print the queue in command line.
 * 
 * @param q The queue.
 * @return 1 on success.
 */
int printQueue(Queue *q){
  while(!empty(q)){
    printBoard(q->front->board);
    q->front = q->front->next;
  }
  return 1;
}
/**
 * Initialise the queue.
 * 
 * @param q The queue.
 * @return 1 on success.
 */
int initQueue(Queue *q){
  q->back = q->front = NULL;
  return 1;
}
/**
 * Check the queue whether it is empty.
 * 
 * @param q The queue.
 * @return 1 on success.
 */
int empty(Queue *q){
  if(q->front==NULL){
    return 1;
  }
  return 0;
}
/**
 * Insert the node to the queue.
 * Check board in the node whether it is repeated.
 * @param q The queue.
 * @param p The node.
 * @return 1 on success. 0 on failure.
 */
int insertNode(Queue *q, Node *p){
  if(q->front == NULL){
    q->front = q->back = p;
    return 0;
  }
  
  if(checkRepeat(*q,p) == 1){
    return 0;
  }
  q->back->next = p;
  q->back = p;
  return 1;
}
/**
 * Remove the node from the queue.
 * 
 * @param q The queue.
 * @return p Return the removed node.
 */
Node *removeNode(Queue *q){
  Node *p;
  p = q->front;
  if(q->front ==NULL){
    return NULL;
  }
  if(q->front == q->back){
    q->front = q->back = NULL;
    return p;
  }
  q->front = p->next;
  return p;
}
/**
 * Compute the correct move by using DFS.
 * It is achieved by using recursions.
 * Explore a board until there are no possible moves.
 * Return to the previous board.
 * Continue to explore until find a correct solution.
 * @param board The board.
 * @param start The node.
 * @return current Return the final node with the flag.
 */
Node *moveDFS(char board[SIZE][SIZE], Node *start){
  int i, j;
  Node *current,*result;
  current = start;
  if(checkWin(board) == SUCCESS){
    current->flag = SUCCESS;
    //printf("---FINISH---\n");
    return current;
  }
  for(j = 0; j < SIZE; j++){
    for(i = 0; i < SIZE; i++){
      if(board[j][i] == PEG){
	if(checkLeft(board, i, j) == GO_LEFT){
	  moveForward(board,GO_LEFT,i,j);
	  current = storeBoard(board, current);
 	  result = moveDFS(board,current);
	  if(result->flag == SUCCESS){return result;}
	  moveBack(board,GO_LEFT,i,j);
	  free(current);
	  current = current->previous;
	}
	if(checkRight(board, i, j) == GO_RIGHT){
	  moveForward(board,GO_RIGHT,i,j);
	  current = storeBoard(board, current);
	  result = moveDFS(board,current);
	  if(result->flag == SUCCESS){return result;}
	  moveBack(board,GO_RIGHT,i,j);
	  free(current);
	  current = current->previous;
	}
	if(checkUp(board, i, j) == GO_UP){
	  moveForward(board,GO_UP,i,j);
	  current = storeBoard(board, current);
	  result = moveDFS(board,current);
	  if(result->flag == SUCCESS){return result;}
	  moveBack(board,GO_UP,i,j);
	  free(current);
	  current = current->previous;
	}
	if(checkDown(board, i, j) == GO_DOWN){
	  moveForward(board,GO_DOWN,i,j);
	  current = storeBoard(board, current);
	  result = moveDFS(board,current);
	  if(result->flag == SUCCESS){return result;}
	  moveBack(board,GO_DOWN,i,j);
	  free(current);
	  current = current->previous;
	}
      }
    }
  }
  current->flag = FAIL;
  return current;
}
/**
 * Store the board in the node.
 * 
 * @param board The board.
 * @param current The node.
 * @return current Return the node which stored the board.
 */
Node *storeBoard(char board[SIZE][SIZE], Node *current){
  Node *new;
  new = AllocateNode(board);
  new->previous = current;
  new->step = current->step;
  current = new;
  current->step++;
  return current;
}
/**
 * Move the peg to the next step according to the direction.
 * 
 * @param board The board.
 * @param direction The direction of move.
 * @param x The row number. 
 * @param y The column number.
 * @return 0 on failure.
 */
int moveForward(char board[SIZE][SIZE], int direction, int x, int y){
  switch(direction){
  case GO_LEFT : 
    board[y][x] = SPACE;
    board[y][x-1] = SPACE;
    board[y][x-2] = PEG; 
    break;
  case GO_RIGHT : 
    board[y][x] = SPACE;
    board[y][x+1] = SPACE;
    board[y][x+2] = PEG; 
    break;
  case GO_UP : 
    board[y][x] = SPACE;
    board[y-1][x] = SPACE;
    board[y-2][x] = PEG; 
    break;
  case GO_DOWN : 
    board[y][x] = SPACE;
    board[y+1][x] = SPACE;
    board[y+2][x] = PEG; 
    break;
  }
  return 0;
}
/**
 * Move the peg to the previous step according to the direction.
 * 
 * @param board The board.
 * @param direction The direction of move.
 * @param x The row number. 
 * @param y The column number.
 * @return 0 on failure.
 */
int moveBack(char board[SIZE][SIZE], int direction, int x, int y){
  switch(direction){
  case GO_LEFT : 
    board[y][x] = PEG;
    board[y][x-1] = PEG;
    board[y][x-2] = SPACE; 
    break;
  case GO_RIGHT : 
    board[y][x] = PEG;
    board[y][x+1] = PEG;
    board[y][x+2] = SPACE; 
    break;
  case GO_UP : 
    board[y][x] = PEG;
    board[y-1][x] = PEG;
    board[y-2][x] = SPACE; 
    break;
  case GO_DOWN : 
    board[y][x] = PEG;
    board[y+1][x] = PEG;
    board[y+2][x] = SPACE; 
    break;
  }
  return 0;
}
/**
 * Check whether the peg can go left.
 * 
 * @param board The board.
 * @param x The row number. 
 * @param y The column number.
 * @return 0 on failure.
 */
int checkLeft(char board[SIZE][SIZE], int x, int y){
  if (x >= 2 && 
      board[y][x-1] == PEG && 
      board[y][x-2] == SPACE){
    return GO_LEFT;
  }
  return 0;
}
/**
 * Check whether the peg can go right.
 * 
 * @param board The board.
 * @param x The row number. 
 * @param y The column number.
 * @return 0 on failure.
 */
int checkRight(char board[SIZE][SIZE], int x, int y){
  if (x <= 2 &&
      board[y][x+1] == PEG && 
      board[y][x+2] == SPACE){
    return GO_RIGHT;
  }
  return 0;
}
/**
 * Check whether the peg can go up.
 * 
 * @param board The board.
 * @param x The row number. 
 * @param y The column number.
 * @return 0 on failure.
 */
int checkUp(char board[SIZE][SIZE], int x, int y){
  if (y >= 2 &&
      board[y-1][x] == PEG && 
      board[y-2][x] == SPACE){
    return GO_UP;
  }
  return FAIL;
}
/**
 * Check whether the peg can go down.
 * 
 * @param board The board.
 * @param x The row number. 
 * @param y The column number.
 * @return 0 on failure.
 */
int checkDown(char board[SIZE][SIZE], int x, int y){
  if (y <= 2 && 
      board[y+1][x] == PEG && 
      board[y+2][x] == SPACE){
    return GO_DOWN;
  }
  return 0;
}
/**
 * Check and count the peg number in the board.
 * 
 * @param board The board.
 * @return count The number of pegs.
 */
int countPegs(char board[SIZE][SIZE]){
  int count, i, j;
  count = 0;
  for(j = 0; j < SIZE; j++){
    for(i = 0; i < SIZE; i++){
      if(board[j][i] == PEG){
	count++;
      }
    }
  }
  return count;
}
/**
 * Initialise the board.
 * Fill the board with char 'i'.
 *
 * @param board The board.
 * @return 1 on success.
 */
int initialise(char board[SIZE][SIZE]){

  int i, j;
  for(j = 0; j < SIZE; j++){
    for(i = 0; i < SIZE; i++){
      board[j][i] = UNFILLED;
    }
  }
  return 1;
}
/**
 * Print out the board in command line.
 * 
 * @param board The board.
 * @return 1 on success.
 */
int printBoard(char board[SIZE][SIZE]){
  int i, j;
  for(j = 0; j < SIZE; j++){
    for(i = 0; i < SIZE; i++){
      printf("%c", board[j][i]);
    }
    printf("\n");
  }
  return 1;
}
/**
 * Print out the moves from the list in command line.
 * 
 * @param current The list.
 * @return 1 on success.
 */
int printSteps(Node *current){
  while(current != NULL){
    printf("##PEGS : %d STEP : %d##\n", countPegs(current->board),current->step);
    printBoard(current->board);
    current = current->previous;
  }
  return 1;
}
/**
 * Allocate the board to the node.
 * initialise the node.
 * @param board The board.
 * @return p Return the initialised node.
 */
Node *AllocateNode(char board[SIZE][SIZE]){
  Node *p;
  p = (Node *)malloc(sizeof(Node));
  if(p==NULL){
    printf("Cannot Allocate Node\n");
    exit(2);
  }
  copyBoard(p->board, board);
  p->previous = NULL;
  p->next = NULL;
  p->step = 0;
  p->flag = FAIL;
  return p;
}
/**
 * Copy the board to a new board.
 * 
 * @param toBoard The destination board.
 * @param fromBoard The source board.
 * @return 1 on success.
 */
int copyBoard(char toBoard[SIZE][SIZE],char fromBoard[SIZE][SIZE]){
  int i,j;
  for(j = 0; j < SIZE; j++){
    for(i = 0; i < SIZE; i++){
      toBoard[j][i] = fromBoard[j][i];
    }
  }
  return 1;
}
/**
 * Reverse a list.
 * 
 * @param list The list.
 * @return list Return the reversed list.
 */
Node *reverseList(Node *list){
  Node *current,*previous,*rest;
  if(list == NULL){
    return list;
  }
  current = list;
  previous = list->previous;
  list->previous = NULL;
  while(previous != NULL){
    rest = previous->previous;
    previous->previous = current;
    current = previous;
    previous = rest;
  }
  list = current;
  return list;
}
/**
 * Draw the correct moves stored in the list with SDL.
 * 
 * @param start The list with the correct solution.
 * @return 1 on success.
 */
int drawMove(Node* start){
  SDL_Simplewin sw;
  Node *current;
  current = start;
  Neill_SDL_Init(&sw);
  do{
    SDL_Delay(MILLISECONDDELAY);
    SDL_RenderClear(sw.renderer);
    /* Draw the step*/
    drawBoard(current->board, sw);      
    /* Update window */
    SDL_RenderPresent(sw.renderer);
    SDL_UpdateWindowSurface(sw.win);
    /* Move to next step */
    if(current->previous != NULL){
      current = current->previous;
    }
    Neill_SDL_Events(&sw);
  }while(!sw.finished);
  atexit(SDL_Quit);
  return 1;
}
/**
 * Draw the board.
 * Pegs are drawn in black circle
 * Spaces are drawn in while rectangle.
 * 
 * @param board The destination board.
 * @param sw The SDL window.
 * @return 1 on success.
 */
int drawBoard(char board[SIZE][SIZE], SDL_Simplewin sw){
  SDL_Rect rectangle;
  int block, unfilled, i, j, radius, circleX, circleY;
  for(j = 0; j < SIZE; j++){
    for(i = 0; i < SIZE; i++){
	  /* Compute block size, circle radius */
      block = WHEIGHT / SIZE;
      unfilled = (WWIDTH - (block * SIZE)) / 2;
      radius  = block / 2;
      rectangle.w = block;
      rectangle.h = block;
      rectangle.x = block * i + unfilled;
      rectangle.y = block * j;
      circleX = rectangle.x + radius;
      circleY = rectangle.y + radius;

      if(board[j][i] == SPACE){
	/* Filled white rectangle */
	Neill_SDL_SetDrawColour(&sw, WHITE, WHITE, WHITE);
	SDL_RenderFillRect(sw.renderer, &rectangle);
      }
      else if(board[j][i] == PEG){
	/* Filled white rectangle as background */
	Neill_SDL_SetDrawColour(&sw, WHITE, WHITE, WHITE);
	SDL_RenderFillRect(sw.renderer, &rectangle);
	/* Filled black circle */
	Neill_SDL_SetDrawColour(&sw, BLACK, BLACK, BLACK);
	Neill_SDL_RenderFillCircle(sw.renderer, circleX, circleY, radius);
      }
      else{
	Neill_SDL_SetDrawColour(&sw, GREY, GREY, GREY);
	SDL_RenderFillRect(sw.renderer, &rectangle);
      }
      /* Unfilled grey rectangle */      
      Neill_SDL_SetDrawColour(&sw, GREY, GREY, GREY);
      SDL_RenderDrawRect(sw.renderer, &rectangle);
      Neill_SDL_SetDrawColour(&sw, BLACK, BLACK, BLACK);
    }
  }
  return 1;
}

