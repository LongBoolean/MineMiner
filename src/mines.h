/*
   mines.h
Author: Nicholas A. Peterson (LongBoolean)
*/
#ifndef MINES_H
#define MINES_H
#include <stdio.h>
typedef struct 
{
    int covered;
    char flag;
    int mined;
    int number;
}Tile;
typedef enum {PENDING, NEW, WON, LOST}GameState_t;

char file_loc_conf[100];
char file_loc_save[100]; 
unsigned int randSeed = 0;
static int Ysize = 10;
static int Xsize = 5;
int lostX = 0;
int lostY = 0;
int numMines = 5;/*20*20/4;*/
GameState_t currentState = PENDING;
int showflags;

void init();
int importFile_inital();
int importFile_final();
int exportFile();
void placeMines(int numMines);
void calculateNumbers();
void generate();

void printHelp();
void printGameGrid();
void flagTile(int x, int y, char c);
void uncoverTile(int x, int y);
void checkWon();

#endif
