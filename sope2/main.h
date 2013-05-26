#ifndef _MAIN_
#define _MAIN_

#include "shdata.h"

/* Handlers */
void *player_kbd_handler(void *arg);
void *player_gameplay_handler(void *arg);
void *player_turn_wait(void *arg);
void *dealer_handler(void *arg);

/* Writes to filename what happened */
int log_game(char *what, char *result, int pnumber, char *name, char *filename);

void fill_str(int spaces, char *str, char *str_to_fill);
char *gettime();
void change_turn(int nplayers, int *turn, long int *timer);

#endif
