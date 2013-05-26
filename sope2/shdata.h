
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
pthread_cond_t cvar;
pthread_cond_t cvar2;
pthread_cond_t ctable;
pthread_cond_t clog;

pthread_mutex_t mut2;
pthread_mutex_t tablemut;
pthread_mutex_t logmut;
pthread_mutex_t mut;

int nplayers;
int in;
player_entry players[9];
int turn;
char room[20];
char filename[20];
int roundnumber;
int dealer;
card cards[52];
int deck_size;
int failed;
int changed;
int roomsize;
char tablecards[9][4];
int tablein;
long int timer,roundtimer;
char lastround[50];
struct tm turn_time;

}shdata;

int file_exist (char *filename);
shdata *joinroom(char *name, char *room, int room_size, int *shmid,int *ownNUMBER, char *ownFIFO, int *fd_read, int *isDealer);
int create_fifo(shdata *addr, char *ownFIFO, int *fd_read);
int cleanall(shdata *addr, int shmid, int *fd_write);
void add_player_to_shdata(shdata *data,char* name,int *ownNUMBER);
void initalize_data(shdata *data, int room_size);
void init_sync_objects_in_shared_memory(shdata *data);
int play_card(char* card,char handcards[6][4],char *hand_str,int *size,shdata *addr);


#endif