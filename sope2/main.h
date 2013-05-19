
#ifndef MAIN
#define MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct card{
char rank[2];
char suit;
}card;


/*array with players' info, holding for each entry (player) a structure with her
number, nickname, FIFOname, etc*/
typedef struct player_entry{
char nickname[15];
int number;
char FIFOname[15];
}player_entry;



typedef struct shdata{
int nplayers;
int in;
player_entry players[10];
int turn;
int roundnumber;
int dealer;
card card[52];
/*mutexes*/

}shdata;
/*
array with players' info, holding for each entry (player) a structure with her
number, nickname, FIFOname, etc.
Note: all users' processes will be able to read the array; each user's process
should only write in its entry.

number of players (nplayers)
turn to play (turn, corresponding to the player's number, 0..nplayers-1)

round number (roundnumber)

dealer's number identification (dealer is 0)

cards on the table, for each round (tablecards)

necessary inter-process synchronization variables (mutexes, condition variables)in order to avoid busy waiting


*/



#endif