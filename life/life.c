/**
 * @file life.c
 * @author YAN SUN
 * @version 1.0
 *
 * @section DESCRIPTION
 * This program implements Conway's famous Game of Life written in C.
 * This program makes use of the SDL to provide a cross-platform 
 * graphical frontend for this application.
 */
#include <stdio.h>
#include <stdlib.h>
#include "neillsdl2.h"

#define ALIVE '#'
#define DEAD '-'
#define UNFILLED 'i'
#define MAX_SIZE 50
#define WHITE 255
#define BLACK 0
#define GREY 192
#define MILLISECONDDELAY 200

void initial(char iniBoard[MAX_SIZE][MAX_SIZE],
	    int status[MAX_SIZE][MAX_SIZE]);
void inputBoard(int x, int y,
	       char board[y][x],
	       char iniBoard[MAX_SIZE][MAX_SIZE]);
void nextGen(int x, int y, 
	    char setBoard[MAX_SIZE][MAX_SIZE],
	    int status[MAX_SIZE][MAX_SIZE]);
void getNeighbours(int x, int y, 
		  char chkBoard[MAX_SIZE][MAX_SIZE], 
		  int status[MAX_SIZE][MAX_SIZE]);
void drawBoard(char iniBoard[MAX_SIZE][MAX_SIZE],
	      SDL_Simplewin sw, SDL_Rect rectangle, 
	     int x, int y);

int main(int argc, char *argv[]){
  int x, y, i, j;
  FILE *file;
  char iniBoard[MAX_SIZE][MAX_SIZE];
  int status[MAX_SIZE][MAX_SIZE];
  SDL_Simplewin sw;
  SDL_Rect rectangle;

  file = NULL;
  Neill_SDL_Init(&sw);
  initial(iniBoard, status);
  file = fopen(argv[1], "r");
  /* fopen returns NULL pointer on failure */
  if (file == NULL){
    printf("Could not open file. \n");
  }
  else {
    printf("File (%s) opened. \n", argv[1]);
    fscanf(file, "%d %d", &x, &y);
    printf("Row: %d Column: %d \n", x, y);
    char board[y][x];
    if(fgetc(file) != EOF){
      for(j = 0; j < y; j++){
	for(i = 0; i < x + 1; i++){
	  board[j][i] = fgetc(file);
	}
      }
    }
    /* Closing file */
    fclose(file);
    /* Input the board which read from the txt file to a 50x50 board*/
    inputBoard(x, y, board, iniBoard);
    do{
      /* Sleep for a short time */
      SDL_Delay(MILLISECONDDELAY);
      SDL_RenderClear(sw.renderer);   
      /* Draw the actual board */
      drawBoard(iniBoard, sw, rectangle, x, y);      
      /* Update window */
      SDL_RenderPresent(sw.renderer);
      SDL_UpdateWindowSurface(sw.win); 
      /* Get the neighbours' status */
      getNeighbours(x, y, iniBoard, status);
      /* Calculate next generation */
      nextGen(x, y, iniBoard, status);
      Neill_SDL_Events(&sw);
    }while(!sw.finished);
    /* Clear up graphics subsystems */
    atexit(SDL_Quit);
  }
  return 0;
}
/**
 * Count the neighbours around the current point.
 * Store the number of neighours in a 2D array.
 * 
 * @param x Number of rows
 * @param y Number of columns
 * @param chkBoard Board
 * @param status Status of points
 */
void getNeighbours(int x, int y, 
		  char chkBoard[MAX_SIZE][MAX_SIZE], 
		  int status[MAX_SIZE][MAX_SIZE]){
  int count, i, j;
  for(j = 0; j < y; j++){
    for(i = 0; i < x; i++){
      count = 0;
      if(chkBoard[j-1][i-1] == ALIVE){count++;}
      if(chkBoard[j-1][i] == ALIVE){count++;}
      if(chkBoard[j-1][i+1] == ALIVE){count++;}
      if(chkBoard[j][i-1] == ALIVE){count++;}
      if(chkBoard[j][i+1] == ALIVE){count++;}
      if(chkBoard[j+1][i-1] == ALIVE){count++;}
      if(chkBoard[j+1][i] == ALIVE){count++;}
      if(chkBoard[j+1][i+1] == ALIVE){count++;}
      status[j][i] = count;
    }
  }
}
/**
 * Calculate the next generation by 
 * checking the status of all the points.
 * 
 * @param x Number of rows
 * @param y Number of columns
 * @param setBoard Board
 * @param status Status of points
 */
void nextGen(int x, int y, 
	    char setBoard[MAX_SIZE][MAX_SIZE],
	    int status[MAX_SIZE][MAX_SIZE]){
  int i, j;
  for(j = 0; j < y; j++){
    for(i = 0; i < x; i++){
      if(setBoard[j][i] == ALIVE && 
	 status[j][i] == (2 || 3)){
	setBoard[j][i] = ALIVE;
      }
      if(setBoard[j][i] == ALIVE &&
	 (status[j][i] < 2 || status[j][i] > 3)){
	setBoard[j][i] = DEAD;
      }
      if(setBoard[j][i] == DEAD &&
	 status[j][i] == 3){
	setBoard[j][i] = ALIVE;
      }
    }
  }
}
/**
 * Input the board which read from the txt file to a 50x50 board
 * 
 * @param x Number of rows
 * @param y Number of columns
 * @param board Board read from the txt file
 * @param iniBoard Board
 */
void inputBoard(int x, int y,
	       char board[y][x],
	       char iniBoard[MAX_SIZE][MAX_SIZE]){
  int i, j;
  for(j = 0; j < y; j++){
    for(i = 0; i < x; i++){
      iniBoard[j][i] = board[j][i];
    }
  }
}
/**
 * Initialise the board with char 'i'
 * Initialise the status with number 0
 *
 * @param iniBoard Board
 * @param status Status of points
 */
void initial(char iniBoard[MAX_SIZE][MAX_SIZE],
	    int status[MAX_SIZE][MAX_SIZE]){
  int i, j;
  for(j = 0; j < MAX_SIZE; j++){
    for(i = 0; i < MAX_SIZE; i++){
      iniBoard[j][i] = 'i';
      status[j][i] = 0;
    }
  }
}
/**
 * Draw the actual board.
 * Draw the alive cell in black
 * Draw the dead cell in white
 * Draw the unfilled area in grey
 *
 * @param iniBoard Board
 * @param sw Window
 * @param rectangle Rectangle
 * @param x Number of rows 
 * @param y Number of columns
 */
void drawBoard(char iniBoard[MAX_SIZE][MAX_SIZE],
	      SDL_Simplewin sw, SDL_Rect rectangle, 
	      int x, int y){
  int block, i ,j;
  for(j = 0; j < MAX_SIZE; j++){
    for(i = 0; i < MAX_SIZE; i++){
      if(iniBoard[j][i] == DEAD){
	Neill_SDL_SetDrawColour(&sw, WHITE, WHITE, WHITE);
      }
      if(iniBoard[j][i] == ALIVE){
	Neill_SDL_SetDrawColour(&sw, BLACK, BLACK, BLACK);
      }
      if(iniBoard[j][i] == UNFILLED){
	Neill_SDL_SetDrawColour(&sw, GREY, GREY, GREY);
      }
      block = WHEIGHT / y;
      rectangle.w = block;
      rectangle.h = block;
      /* Filled Rectangle */
      rectangle.x = block * i;
      rectangle.y = block * j;
      SDL_RenderFillRect(sw.renderer, &rectangle);
      /* Unfilled Rectangle */
      Neill_SDL_SetDrawColour(&sw, GREY, GREY, GREY);
      rectangle.x = block * i;
      rectangle.y = block * j;
      SDL_RenderDrawRect(sw.renderer, &rectangle);
    }
  }
}
