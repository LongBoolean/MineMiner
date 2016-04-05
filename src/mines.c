/*mines.c
 Author: Nicholas A. Peterson (LongBoolean)
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "mines.h"
/*Mine Miner Features plan:
	-start a new game
		$ mm --newgame
		or
		$ mm --newgame x y 
		or
		$ mm --newgame x y mines
		or
		$ mm --newgame x y --diff 0,1,2,3-10
	-print tile array state
		$ mm -p 
		or
		$ mm -p --noflags
	-flag tiles
		$ mm -f x y 		set or remove flag
		or
		$ mm -f x y 'c'		set flag charactor
	-uncover tiles
		$ mm -u x y 

Process:
	-load game file

	-parse input
		if(new_game) set up new game
		if(flag) edit flag array tile
		if(uncover) edit covered array

	-save game file

Arrays:
	bool covered
	char flags
	bool mines
	int nums

Game Modes:
	setup new game
	generate new game and pick fist tile
	play game
	game lost/ game won
	

*/

Tile grid[20][20];


int main(int argc, char** argv)
{
    //setup file locations
    strcpy(file_loc_conf,"../conf/mineminer.conf");
    strcpy(file_loc_save,"../conf/save.mmg");

    currentState = NEW;
    showflags = 1;
    printf("\nMine Miner\n");
    if(argc < 2)
    {
        printf("Please use --help for instructions.\n");
    }
    int i;
    int selectX;
    int selectY;
    char selectC;
    //pre arguments
    int keepgoing = 1;
    int final_import = 1;
    int inital_import = 1;
    for(i=0; i<argc; i=i+1)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case '-':
                    if(argv[i][2] == 'h' && 
                            argv[i][3] == 'e' && 
                            argv[i][4] == 'l' && 
                            argv[i][5] == 'p')
                    {
                        printHelp();
                        keepgoing = 0;
                    }
                    else if(argv[i][2] == 'n' && 
                            argv[i][3] == 'o' && 
                            argv[i][4] == 'f' && 
                            argv[i][5] == 'l'&& 
                            argv[i][6] == 'a' && 
                            argv[i][7] == 'g' && 
                            argv[i][8] == 's')
                    {
                        showflags = 0;
                    }
                    else if(argv[i][2] == 'n' && 
                            argv[i][3] == 'e' && 
                            argv[i][4] == 'w' && 
                            argv[i][5] == 'g'&& 
                            argv[i][6] == 'a' && 
                            argv[i][7] == 'm' && 
                            argv[i][8] == 'e')
                    {
                        printf("New Game Created\n");
                        currentState = NEW;
                        numMines = 20*20/16;
                        Ysize = 20;
                        Xsize = 15;
                        keepgoing = 1;
                        final_import = 0;
                        inital_import = 0;
                    }
                    break;
            }
        }
    }
    //set up the game grid
    if(keepgoing==1)
    {
        if(inital_import==1)
        {
            importFile_inital();
        }
        generate();
        if(final_import==1)
        {
            importFile_final();
        }
    }
    //other input
    for(i=0; keepgoing==1 && i<argc; i=i+1)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'u':
                    selectX = atoi(argv[++i]);
                    selectY = atoi(argv[++i]);
                    uncoverTile(selectX,selectY);
                    break;
                case 'f':
                    selectX = atoi(argv[++i]);
                    selectY = atoi(argv[++i]);
                    char flag = 'f';
                    if(i+1<argc && argv[i+1][0] == '_')
                    {
                        flag = argv[i+1][1];
                        ++i;
                    }
                    flagTile(selectX,selectY,flag);
                    break;

                case 'p':
                    printGameGrid();
                    break;
                case '-':
                    //ignore these, they have already been handled
                    break;
                default:
                    printf("Invallid argument %s\n",argv[i]);
            } 
        } 
    } /*end testing*/ 

    //export test
    if(keepgoing)
        exportFile(file_loc_save);
    return 0;
} 

int importFile_inital() 
{ 
    FILE *rf_save = NULL; 
    rf_save = fopen(file_loc_save, "r");
    if(rf_save == NULL)
        return 0;
    //read file line by line
    int max_line_length = 512;
    char lineBuffer [max_line_length];
    int count = 0;
    int tempInt = 0;
    while(fgets(lineBuffer, max_line_length, rf_save) != NULL)
    {
        if(lineBuffer[0] != '#')
        {
            switch(count)
            {
                case 0: //import gamestate
                    sscanf(lineBuffer, "%d", &tempInt);
                    //if(tempInt == PENDING)
                        currentState = tempInt;
                    //else
                     //   printf("Creating New Game\n");
                    //printf("GameState: %d\n",currentState);
                    count++;
                    break;
                case 1: //import seed
                    sscanf(lineBuffer, "%d", (int*)&randSeed);
                    //printf("ImportSeed: %d\n",randSeed);
                    count++;
                    break;
                case 2: //import grid size x y 
                    sscanf(lineBuffer, "%d %d", &Xsize, &Ysize);
                    //printf("Xsize:%d  Ysize:%d\n",Xsize, Ysize);
                    count++;
                    break;
                case 3: //import lostX and lostY
                    sscanf(lineBuffer, "%d %d", &lostX, &lostY);
                    //printf("LostX:%d  LostY:%d\n",lostX, lostY);
                    count++;
                    break;
                case 4: //import num mines
                    sscanf(lineBuffer, "%d", &numMines);
                    //printf("NumMines: %d\n", numMines);
                    count++;
                    break;
                default:
                    break;
            }

        }
    }

    fclose(rf_save);
    return 1; 
}

int importFile_final() 
{ 
    FILE *rf_save = NULL; 
    rf_save = fopen(file_loc_save, "r");
    if(rf_save == NULL)
        return 0;
    //read file line by line
    int max_line_length = 512;
    char lineBuffer [max_line_length];
    int count = 0;
    int i = 0;
    while(fgets(lineBuffer, max_line_length, rf_save) != NULL)
    {
        if(lineBuffer[0] != '#')
        {
            switch(count)
            {
                case 5: //import covered tiles
                    i = 0;
                    do
                    {
                        int j;
                        for(j=0;j<Xsize;j++)
                        {
                            if(lineBuffer[j]=='0')
                                uncoverTile(j, i);
                            //printf("%c", lineBuffer[j]);
                        }
                        //printf("\n");
                        i++;
                    }
                    while (i<Ysize && fgets(lineBuffer, max_line_length, rf_save) != NULL);
                    count++;
                    //printf("/////////////////////////////////\n");
                    break;
                case 6://import flagged tiles
                    i = 0;
                    do
                    {
                        int j;
                        for(j=0;j<Xsize;j++)
                        {
                            if(lineBuffer[j]!='-')
                                flagTile(j, i,lineBuffer[j]);
                            //printf("%c", lineBuffer[j]);
                        }
                        //printf("\n");
                        i++;
                    }
                    while(i<Ysize && fgets(lineBuffer, max_line_length, rf_save) != NULL);
                    count++;
                    break;
                default:
                    //			printf("lol Im here %d\n", count);
                    count++;
                    break;
            }

        }
    }

    fclose(rf_save);
    return 1; 
}

int exportFile()
{
    FILE *wf_save;
    wf_save = fopen(file_loc_save, "w");
    fprintf(wf_save,"#MineMiner Save File\n");
    fprintf(wf_save,"#If you edit this file the program will likly become buggy and all bets are off\n");
    //export gamestate pending = 0, new = 1, won = 2, lost = 3
    fprintf(wf_save,"%d\n", currentState);
    //export random seed
    fprintf(wf_save,"%d\n", randSeed);
    //export grid size x and y
    fprintf(wf_save,"%d %d\n", Xsize, Ysize);
    //export lostX lostY
    fprintf(wf_save,"%d %d\n", lostX, lostY);
    //export numMines
    fprintf(wf_save,"%d\n", numMines);
    //export covered tiles
    int i = 0; 
    int j = 0;
    for(i=0;i < Ysize; i=i+1)
    {
        for(j=0;j < Xsize; j=j+1)
        {
            if(grid[i][j].covered)
                fprintf(wf_save,"%d", 1);
            else
                fprintf(wf_save,"%d", 0);
        }
        fprintf(wf_save,"\n");
    }

    //export flaged tiles
    i = 0; 
    j = 0;
    for(i=0;i < Ysize; i=i+1)
    {
        for(j=0;j < Xsize; j=j+1)
        {

            if(grid[i][j].flag == ' ')
                fprintf(wf_save,"%c", '-');
            else
                fprintf(wf_save,"%c", grid[i][j].flag);
        }
        fprintf(wf_save,"\n");
    }


    fclose(wf_save);
    return 1;
}
void generate()
{
    init();
    placeMines(numMines);
    calculateNumbers();
}
void init()
{
    int i;
    int j;
    for(i=0;i < Ysize; i=i+1)
    {
        for(j=0;j < Xsize; j=j+1)
        {
            grid[i][j].covered = 1;
            grid[i][j].flag = ' ';
            grid[i][j].mined = 0;
            grid[i][j].number = 0;

        }
    }

}
void placeMines(int numMines)
{
    if(currentState == NEW)
    {
        randSeed = (unsigned int) time(NULL);
        currentState = PENDING;
    }
    srand(randSeed);
    int x = 0;
    int y = 0;
    int i;
    for(i=0;i<numMines;)
    {
        x = rand() % Xsize;
        y = rand() % Ysize;
        if(grid[y][x].mined == 0)
        {
            grid[y][x].mined = 1;
            i=i+1;
        } 
    }
}
void calculateNumbers()
{
    int i;
    int j;
    int count = 0;
    for(i=0;i < Ysize; i=i+1)
    {
        for(j=0;j < Xsize; j=j+1)
        {
            /*corners*/
            if(i>0 && j>0 && grid[i-1][j-1].mined)
            {
                count = count + 1;
            }
            if(i<Ysize && j<Xsize && grid[i+1][j+1].mined)
            {
                count = count + 1;
            }
            if(i>0 && j<Xsize && grid[i-1][j+1].mined)
            {
                count = count + 1;
            }
            if(i<Ysize && j>0 && grid[i+1][j-1].mined)
            {
                count = count + 1;
            }
            /*sides*/
            if(i>0 && grid[i-1][j].mined)
            {
                count = count + 1;
            }
            if(i<Ysize && grid[i+1][j].mined)
            {
                count = count + 1;
            }
            if(j>0 && grid[i][j-1].mined)
            {
                count = count + 1;
            }
            if(j<Xsize && grid[i][j+1].mined)
            {
                count = count + 1;
            }
            grid[i][j].number = count;
            count = 0;
        }
    }
}
void flagTile(int x, int y, char c)
{
    if(x<Xsize && x>=0 && y<Ysize && y>=0)
    {
        grid[y][x].flag = c;
    }
    else
    {
        printf("\nFlag Error: Coordinates out of bounds error.\n");
    }

}
void uncoverTile(int x, int y)
{
    if(x<Xsize && x>=0 && y<Ysize && y>=0)
    {
        if(grid[y][x].covered == 0)
            return;
        //printf("Uncover Tile %d %d \n", x, y);
        if(currentState != LOST)
        {
            grid[y][x].covered = 0;
            if(grid[y][x].mined == 1)
            {
                currentState = LOST;
                lostX = x;
                lostY = y;
            }
            /*uncover all tiles around a tile number of 0*/
            if(grid[y][x].number == 0 && !grid[y][x].mined)
            {
                /*corners*/
                if(y>0 && x>0 && grid[y-1][x-1].covered)
                {
                    uncoverTile(x-1,y-1);
                }
                if(y<Ysize && x<Xsize && grid[y+1][x+1].covered)
                {
                    uncoverTile(x+1,y+1);
                }
                if(y>0 && x<Xsize && grid[y-1][x+1].covered)
                {
                    uncoverTile(x+1,y-1);
                }
                if(y<Ysize && x>0 && grid[y+1][x-1].covered)
                {
                    uncoverTile(x-1,y+1);
                }
                /*sides*/
                if(y>0 && grid[y-1][x].covered)
                {
                    uncoverTile(x,y-1);
                }
                if(y<Ysize && grid[y+1][x].covered)
                {
                    uncoverTile(x,y+1);
                }
                if(x>0 && grid[y][x-1].covered)
                {
                    uncoverTile(x-1,y);
                }
                if(x<Xsize && grid[y][x+1].covered)
                {
                    uncoverTile(x+1,y);
                }
            }
        }
    }
    else
    {
        printf("\nUncover Error: Coordinates out of bounds error. x:%d y:%d\n", x, y);
    }
}
void checkWon()
{
    int i;
    int j;
    int count = 0;
    for(i=0;i < Ysize; i=i+1)
    {
        for(j=0;j < Xsize; j=j+1)
        {
            if(!grid[i][j].covered)
            {
                count = count + 1;
            }
        }
    }
    if(Xsize*Ysize - numMines <= count)
    {
        currentState = WON;
    }

}
void printHelp()
{
    printf("Mine Miner Help\n"
            "Note: This game is not yet completed.\n"
            "Note: Some arguments listed do not work at the moment.\n"
            "start a new game\n"
            "\t$ mm --newgame\n"
            "\tor\n"

            "\t$ mm --newgame x y \n"
            "\tor\n"
            "\t$ mm --newgame x y mines\n"
            "\tor\n"
            "\t$ mm --newgame x y --diff 0,1,2,3-10\n"
            "print game grid\n"
            "\t$ mm -p \n"
            "\tor\n"
            "\t$ mm -p --noflags\n"
            "set flag tiles\n"
            "\t$ mm -f x y\tset or remove flag\n"
            "\tor\n"
            "\t$ mm -f x y 'c'\tset flag charactor\n"
            "uncover tiles\n"
            "\t$ mm -u x y \n");
}
void printGameGrid()
{
    printf("\nPrint Grid\n");
    /*set Covered tiles to 1 */
    int i;
    int j;
    int k;
    if(Ysize>10) printf(" ");
    printf("   ");
    for(k=0;k<Xsize; k=k+1)
    {
        int val = ((k % 100)-(k % 10))/10;
        if(val > 0)
        {
            printf("%d ", val);
        }
        else
        {
            printf("  ");
        }
    }
    printf("\n");



    if(Ysize>10) printf(" ");
    printf(" X ");
    for(k=0;k<Xsize; k=k+1)
    {
        printf("%d ", k % 10);
    }
    printf("\n");



    if(Ysize>10) printf(" ");
    printf("Y# ");
    for(k=0;k<Xsize; k=k+1)
    {
        printf("# ");
    }
    printf("\n");

    for(i=0;i < Ysize; i=i+1)
    {
        if(Ysize>10 && i<10) printf(" ");
        printf("%d# ", i);
        for(j=0;j < Xsize; j=j+1)
        {
            if(grid[i][j].covered == 0 )
            {
                if(currentState == LOST && lostX == j && lostY == i)
                {
                    printf("M ");
                }
                else if(currentState == LOST && grid[i][j].mined ==1)
                {
                    printf("m ");
                }
                else if(grid[i][j].number == 0)
                {
                    printf("  ");
                }
                else
                {
                    printf("%d ", grid[i][j].number);
                }
            }
            else if(currentState == LOST && grid[i][j].mined == 1)
            {
                printf("m ");
            }
            else if(showflags && grid[i][j].flag != ' ')
            {
                if(currentState == LOST && !grid[i][j].mined)
                    printf("%c ", 'X');
                else
                    printf("%c ", grid[i][j].flag);
            }
            else
            {
                printf("- ");
            }
        }
        printf("\n");
        if(currentState == WON)
        {
            printf("Congrats, you uncovered all of the mines.\n");
        }

    }

}
