/**
 * @file treevis.c
 * @author YAN SUN
 * @version 1.0
 *
 * @section DESCRIPTION
 * This program implements a tree visualisation program written in C.
 * Input a correct expression, the program will show the HV tree.
 * This program makes use of the SDL to provide a graphical interface for this application.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neillsdl2_font.h"
#define RIGHT_LINE "-"
#define DOWN_LINE "|\n|\n|\n|\n"
#define SIZE 1.5;
#define INI_X 50
#define INI_Y 50
#define MILLISECONDDELAY 500
#define WHITE 255
#define MAX_LEN 5
#define DOWN 1
#define RIGHT 2
#define NULL_NODE '*'

struct node{
  char string[MAX_LEN];
  int x;
  int y;
  int d;  /* Direction of the node */
  int rightLen;
  int downLen;
  struct node *left;
  struct node *right;
  struct node *previous;
};
typedef struct node Node;

Node *AllocateNode(char *string);
int checkStr(char *str, int i, char new[MAX_LEN]);
Node *createTree(Node *start, char *str);
int draw(Node *current, int rightLen, int downLen, SDL_Simplewin sw);
int drawTree(Node *start);
int drawLine(int num, char *str);
int checkBracket(int l,int r);

int main(int argc, char *argv[]){
  char *str;
  Node *start;
  start = NULL;
  str = argv[1];
  /* Create the tree according to the input string */
  start = createTree(start,str);
   /* Draw the tree with SDL */
  drawTree(start);
  return 1;
}
/**
 * Create the tree according to the input string.
 * Record the number of brackets.
 * 
 * @param start The initial node.
 * @param str The input string get from argv.
 * @return start The node of the tree.
 */
Node *createTree(Node *start, char *str){
  char new[MAX_LEN];
  int length, i,leftBracket,rightBracket;
  Node *current, *newNode;
  length = strlen(str);
  current = NULL;
  leftBracket = 0;
  rightBracket = 0;
  for(i = 0; i<length; i++){
    if(str[i] !='(' && str[i] != ')'){
      i = checkStr(str,i ,new);
      /* If the root node is null, create the root node. */
      if(current == NULL){
	start = AllocateNode(new);
	current = start;
      }
      /* Create the child node. */
      else{
	newNode = AllocateNode(new);
	newNode->previous = current;
	if(current->left == NULL){
	  newNode->d = 1;
	  current->left = newNode;
	}
	else{
	  newNode->d = 2;
	  current->right = newNode;
	}
	current = newNode;
      }
    }
    /* Back to the previous node if there is a right bracket*/
    if(str[i] ==')'){
      current = current->previous;
      rightBracket++;
    }
    if(str[i] == '('){
      leftBracket++;
    }
  }
  checkBracket(leftBracket,rightBracket);
  return start;
}
/**
 * Check whether the number of left brackets is equal to
 * the number of right brackets, and also check whether 
 * the number of brackets is even.
 * Terminate the program if not.
 * 
 * @param l The number of left brackets.
 * @param r The number of left brackets.
 * @return 1 On success.
 */
int checkBracket(int l,int r){
  if(l != r || l % 2 != 0){
    printf("Invalid input!\n");
    exit(2);
  }
  return 1;
}
/**
 * Get the string which need to put in the node.
 * 
 * @param str The input string.
 * @param i The current position of the string.
 * @return i Return the num i which is the current position of the string.
 */
int checkStr(char *str, int i, char new[MAX_LEN]){
  int j;
  j = 0;
  while(str[i] != '(' && str[i] != ')'){
    new[j] = str[i];
    i++;
    j++;
  }
  i--;
  new[j] = '\0';
  return i;
}
/**
 * Allocate the string to the node.
 * Initialise the node.
 * @param string The string gets from the input string.
 * @return p Return the initialised node.
 */
Node *AllocateNode(char *string){
  Node *p;
  p = (Node *)malloc(sizeof(Node));
  if(p==NULL){
    printf("Cannot Allocate Node\n");
    exit(2);
  }
  strcpy(p->string, string);
  p->left = NULL;
  p->right= NULL;
  p->previous= NULL;
  p->x = 0;
  p->y = 0;
  p->d = 0;
  p->rightLen = 0;
  p->downLen = 0;
  return p;
}
/**
 * Draw the tree with SDL according to the input root node.
 * @param start The root node of the tree.
 * @return 1 Return 1 on success.
 */
int drawTree(Node *start){
  SDL_Simplewin sw;
  int rightLen, downLen;
  rightLen = 0;
  downLen = 0;
  Neill_SDL_Init(&sw);
  SDL_RenderClear(sw.renderer);
  /* Draw the whole tree. */
  draw(start, rightLen, downLen, sw);
  SDL_RenderPresent(sw.renderer);
  SDL_UpdateWindowSurface(sw.win);
  do{
    Neill_SDL_Events(&sw);
  }while(!sw.finished);
  atexit(SDL_Quit);
  return 1;
}
/**
 * Draw each node with horizontal line and vertical line.
 * 
 * @param current The current node.
 * @param rightLen the number of encountered right node. 
 *        Use this number to decide the position.
 * @param downLen The depth of the node.
 *        Use this number to decide the position.
 * @param sw The windows.
 * @return rightLen Return the number of the encountered right node.
 */
int draw(Node *current, int rightLen, int downLen, SDL_Simplewin sw){
  int strLen, x, y, d, i, rightNum, pRightLen;
  /* The postion of the current node. */
  current->x = INI_X + rightLen * FNT_FONTWIDTH*MAX_LEN*SIZE; 
  current->y = INI_Y + downLen * FNT_FONTHEIGHT*MAX_LEN*SIZE;
  current->rightLen = rightLen;
  current->downLen = downLen;
  x = current->x;
  y = current->y;
  d = current->d;
  /* Set the draw colour. */
  Neill_SDL_SetDrawColour(&sw, WHITE, WHITE, WHITE);
 
  if(current->previous!= NULL){
    strLen = strlen(current->previous->string);
    pRightLen = current->previous->rightLen;
    if(d == DOWN){
    /* Draw the vertical line. */
      Neill_SDL_DrawText(&sw,DOWN_LINE, 
			 current->previous->x, 
			 current->previous->y + FNT_FONTHEIGHT * 2);
    }
    if(d == RIGHT){
      rightNum = MAX_LEN - (strLen - 1) + (rightLen - pRightLen - 1)*MAX_LEN*SIZE;
      /* Draw the horizontal line. */
      for(i = 0;i<rightNum;i++){
	Neill_SDL_DrawText(&sw,RIGHT_LINE,
			   current->previous->x+(strLen + i + 1)*FNT_FONTWIDTH, 
			   current->previous->y);
      }
    }
  }
  /* Draw the string of current node. */
  Neill_SDL_DrawText(&sw, current->string, x, y);
  /* This recursion here did a DFS, traverse the whole tree. */
  if(current->left->string[0] != NULL_NODE){
    downLen++;
    rightLen = draw(current->left, rightLen, downLen, sw);
    downLen--;
  }
  if(current->right->string[0] != NULL_NODE){
    rightLen++;
    rightLen = draw(current->right, rightLen, downLen, sw);
  } 
  return rightLen;
}

