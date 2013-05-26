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

/* Initiates the deck with the 52 cards shuffled. */
void init_deck(card *cards, int *deck_size);

/* Extract cards from hand string to handcards. */
void read_hand(char* hand,char handcards[6][4]);

/* Makes a string out of handcards for printing */
void format_hand(char handcards[6][4],int size,char *hand_str);

/* Removes number cards from the top of the deck */
void remove_cards(card *deck,int *deck_size,int number);

/* Shuffles cards. */
void shuffle_deck(card *cards, int deck_size);

/* Gives one card from the top of the deck, and sends it to the player through his FIFO */
void give_card(card *cards,int *deck_size,char *fifo,int player_number, int fd);

/* Initial distribution of cards to each player */
int distributing_cards(card *cards,int *deck_size,char *fifo,int player_number, int fd);



#endif