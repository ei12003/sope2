#ifndef _CARDS_
#define _CARDS_
#define HANDLIMIT 6

#include <string.h>
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
typedef struct card{
char rank[3];
char suit;
}card;

int give_card(card *cards,int *deck_size,char *fifo,int player_number, int fd);
void remove_cards(card *deck,int *deck_size,int number);
void shuffle_deck(card *cards, int deck_size);
int distributing_cards(card *cards,int *deck_size,char *fifo,int player_number, int fd);
void format_hand(char handcards[6][4],int size,char *hand_str);
void read_hand(char* hand,char handcards[6][4]);
void init_deck(card *cards, int *deck_size);
#endif