
#ifndef MAIN
#define MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> 
#include <fcntl.h>   
#include <errno.h>  
#include <termios.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define HANDLIMIT 6

typedef struct card{
char rank[3];
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
pthread_cond_t cvar;
pthread_mutex_t mut;
int nplayers;
int in;
player_entry players[10];
int turn;
int roundnumber;
int dealer;
card cards[52];
int deck_size;
int failed;
/*mutexes*/

}shdata;

void *thr_func(void *arg);


void init_sync_objects_in_shared_memory(shdata *data);
void remove_cards(card *deck,int *deck_size,int number);
int distributing_cards(card *cards,int *deck_size,char *fifo,int player_number);
void print_shdata(shdata data);
void add_player_to_shdata(shdata *data,char* name);
void initalize_data(shdata *data, int room_size);
shdata *joinroom(char *name, char *room, int room_size, int *shmid);
int create_fifo(shdata *addr);
int cleanall(shdata *addr, int shmid);
void shuffle_deck(card *cards,int deck_size);
void init_deck(shdata *addr);
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