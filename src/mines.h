/*
mines.h
Author: Nicholas A. Peterson (LongBoolean)
*/
#ifndef MINES_H
#define MINES_H
#include <stdio.h>
typedef struct 
{
	bool covered;
	char flag;
	bool mined;
	int number;
}Tile;
typedef enum {PENDING, NEW, WON, LOST}GameState_t;

char file_loc_conf[100];
char file_loc_save[100]; 
static int Ysize = 30;
static int Xsize = 30;
int lostX = 0;
int lostY = 0;
int numMines = 30*30/4;
GameState_t currentState = PENDING;
bool showflags;

void init();
bool importFile();
bool exportFile();
void placeMines(int numMines);
void calculateNumbers();
void generate();

void printHelp();
void printGameGrid();
void flagTile(int x, int y, char c);
void uncoverTile(int x, int y);
void checkWon();

#endif
