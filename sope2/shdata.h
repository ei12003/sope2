
#ifndef _SHDATA_
#define _SHDATA_
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
#include "cards.h"

typedef struct player_entry{
char nickname[15];
int number;
char FIFOname[40];
}player_entry;



typedef struct shdata{

int nplayers, in, turn, roundnumber, dealer, deck_size, failed, changed, roomsize, tablein;
char room[20], filename[20], lastround[50], tablecards[9][4];

player_entry players[9];
card cards[52];
long int timer,roundtimer;
struct tm turn_time;

pthread_cond_t cvar, cvar2 ,ctable, clog;
pthread_mutex_t mut2, tablemut, logmut, mut;

}shdata;

/* Initializes all variables from the shared memory*/
void initalize_data(shdata *data, int room_size);

/* Adds player to room */
void add_player_to_shdata(shdata *data,char* name,int *ownNUMBER);

/* Initiate all mutexes and conditional variables */
void init_sync_objects_in_shared_memory(shdata *data);

/* Cleans all memory used by the all players and dealer */
int cleanall(shdata *addr, int shmid, int *fd_write);

/* Creates the player FIFO and opens it for reading. (Created by the player) */
int create_fifo(shdata *addr, char *ownFIFO, int *fd_read);

/* Plays the card to the table in the shared memory */
int play_card(char* card,char handcards[6][4],char *hand_str,int *size,shdata *addr);

/* Joins the room
 * If it doesn't exits, it will create it.
 * Sets all the necessary things for the player.
*/
shdata *joinroom(char *name, char *room, int room_size, int *shmid,int *ownNUMBER, char *ownFIFO, int *fd_read, int *isDealer);

/* Checks if the log existis */
int file_exist (char *filename);







#endif