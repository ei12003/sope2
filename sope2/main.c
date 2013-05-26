#include "main.h"

int isDealer;
int fd_write[10];
int fd_read,ownNUMBER;
char hand[150];
char ownFIFO[40], ownNAME[40];
char handcards[6][4]={{0}};
int handsize=6;

int main(int argc, char *argv[], char *envp[])
{
shdata *addr;
pthread_t tidp_gt,tidp_gp,tidp_kbd,tidd;
int shmid=0;

 if(argc!=4){
        printf("Invalid number of arguments\n");
        return -1;
    }

    if(52%atoi(argv[3])!=0){
        printf("Room size must be either 2 or 4.\n");
        return -1;
    }
    
    /* Setting up shared memory (creating or joining) */
    addr=joinroom(argv[1],argv[2],atoi(argv[3]),&shmid,&ownNUMBER,ownFIFO,&fd_read,&isDealer);

    if(addr[0].failed==1){
        printf("\n##########\nRoom \"%s\" is full.\n", argv[2]);
        return -1;
    }

    else if(addr[0].failed==2){
        printf("\n##########\nCouldn't create FIFO.\n");
        return -2;
    }


    strcpy(ownNAME,argv[1]);

    if(isDealer==1){
        init_deck(addr[0].cards,&addr[0].deck_size);
        init_sync_objects_in_shared_memory(addr);
        pthread_create(&tidd, NULL, dealer_handler, addr);        
        }
        

    pthread_create(&tidp_gp,NULL,player_gameplay_handler,addr);
    pthread_create(&tidp_gt, NULL, player_turn_wait, addr); 
    pthread_create(&tidp_kbd, NULL, player_kbd_handler, addr);
    
    /* Wait for player to type "exit" */
    pthread_join(tidp_kbd, NULL);

    close(fd_read);
    
    if(isDealer==1)
       cleanall(addr,shmid,fd_write);
    else
        shmdt(addr);

return 0;
}

/* ##################
        Functions
   ################## */

void *player_kbd_handler(void *arg){
        shdata *addr=(shdata*)arg;
        char ch[100], aux[15], card[4];
        int c;
        
        while(1){
           printf("%s > ",ownNAME);
            scanf("%s",ch);
            
            if(strcmp(ch,"hand")==0){
                pthread_mutex_lock(&addr[0].logmut); 
                log_game("hand", hand,ownNUMBER,ownNAME,addr[0].filename);
                pthread_mutex_unlock(&addr[0].logmut); 
                printf("[Hand: %s]\n",hand);
            }
            else if(strcmp(ch,"round-time")==0){
                printf("[%lu seconds have passed since the beginning of the round.]\n",time(NULL)-addr[0].roundtimer);
                pthread_mutex_lock(&addr[0].logmut); 
                memset(aux,'\0',sizeof(aux));
                sprintf(aux,"%lu",time(NULL)-addr[0].roundtimer);
                strcat(aux," secs");
                log_game("round-time",aux,ownNUMBER,ownNAME,addr[0].filename);
                pthread_mutex_unlock(&addr[0].logmut); 
            }
            else if(strcmp(ch,"turn-time")==0){
                printf("[%lu seconds have passed since the beginning of the turn.]\n",time(NULL)-addr[0].timer);
                pthread_mutex_lock(&addr[0].logmut); 
                memset(aux,'\0',sizeof(aux));
                sprintf(aux,"%lu",time(NULL)-addr[0].timer);
                strcat(aux," secs");
                log_game("turn-time",aux,ownNUMBER,ownNAME,addr[0].filename);
                pthread_mutex_unlock(&addr[0].logmut); 
            }
            else if(strcmp(ch,"round")==0){
                printf("[Round: %d]\n",addr[0].roundnumber);
                pthread_mutex_lock(&addr[0].logmut); 
                memset(aux,'\0',sizeof(aux));
                sprintf(aux,"%d",addr[0].roundnumber);
                log_game("round",aux,ownNUMBER,ownNAME,addr[0].filename);
                pthread_mutex_unlock(&addr[0].logmut); 
            }
            else if(strcmp(ch,"lastround")==0){
                printf("[Lastround: %s]\n",addr[0].lastround);
                pthread_mutex_lock(&addr[0].logmut); 
                log_game("lastround",addr[0].lastround,ownNUMBER,ownNAME,addr[0].filename);
                pthread_mutex_unlock(&addr[0].logmut); 
            }
            else if(strcmp(ch,"table")==0){
                memset(ch,'\0',sizeof(ch));
                format_hand(addr[0].tablecards,addr[0].tablein,ch);
                printf("[Table: %s]\n",ch);
                pthread_mutex_lock(&addr[0].logmut); 
                log_game("table",ch,ownNUMBER,ownNAME,addr[0].filename);
                pthread_mutex_unlock(&addr[0].logmut); 
            }
            else if(strcmp(ch,"turn")==0){
                printf("[Turn: %s]\n",addr[0].players[addr[0].turn].nickname);
                pthread_mutex_lock(&addr[0].logmut); 
                log_game("turn",addr[0].players[addr[0].turn].nickname,ownNUMBER,ownNAME,addr[0].filename);
                pthread_mutex_unlock(&addr[0].logmut); 
            }
            else if(strstr(ch,"play:")!=NULL){
                if(handsize==0)
                    printf("No more cards to play! Please leave!\n");
                else if(addr[0].turn!=ownNUMBER )
                    printf("It's not your turn to play!\n");
                else{
                    c=sscanf(ch,"play:%s",card);

                    c=play_card(card,handcards,hand,&handsize,addr);
                    if(c==-1)
                        printf("You don't have that card\n");
                    else{
                        pthread_mutex_lock(&addr[0].logmut); 
                        log_game("play",card,ownNUMBER,ownNAME,addr[0].filename);
                        pthread_mutex_unlock(&addr[0].logmut); 
                        if(handsize==0)
                            fprintf(stderr,"No more cards to play!\n");
                        change_turn(addr[0].nplayers,&addr[0].turn,&addr[0].timer);
                        addr[0].changed=1;
                        
                        pthread_mutex_lock(&addr[0].mut2);
                        pthread_cond_broadcast(&addr[0].cvar2);
                        pthread_mutex_unlock(&addr[0].mut2); 
                     }
                    
                }
		    }
            else if(strcmp(ch,"exit")==0){
                break;
            }
            else if(strcmp(ch,"end")==0 && isDealer==1){
                addr[0].deck_size=addr[0].roomsize;
            }
	        else{
	           printf("\n\n############\nSupported commands:\nhand | play:Rs | turn\nround | lastround | table\nturn-time| round-time | exit\n\n");
	        }
            printf("\n");
        }

    return 0;
}


void *player_turn_wait(void *arg){
  shdata *addr=(shdata*)arg;

  /* Warns the player when it's his time to play */
    while(addr[0].turn!=-2){
        pthread_mutex_lock(&addr[0].mut2); 
        while (addr[0].turn != ownNUMBER || addr[0].changed==0)
            pthread_cond_wait(&addr[0].cvar2, &addr[0].mut2);
        pthread_mutex_unlock(&addr[0].mut2); 
        addr[0].changed=0;
    if(handsize>0) {
        fprintf (stderr, "\nIt's your turn!\n\n%s > ",ownNAME);
        fflush(stderr);
    }
    else{
       return 0;
    }
    }
    return 0;
}

void *player_gameplay_handler(void *arg){
    shdata *addr=(shdata*)arg;
    char aux[10]="";

    /* Warns the dealer he got in */
    pthread_mutex_lock(&addr[0].mut);
    pthread_cond_signal(&addr[0].cvar);
    pthread_mutex_unlock(&addr[0].mut); 
    
    /* Waits for all players to be in and the cards to be distributed */
    pthread_mutex_lock(&addr[0].mut2); 
    while (addr[0].roundnumber == 0)
        pthread_cond_wait(&addr[0].cvar2, &addr[0].mut2);
    pthread_mutex_unlock(&addr[0].mut2); 
    
    /* Reads his cards */
    read(fd_read,hand,sizeof(hand));
    read_hand(hand,handcards);
    
    /* Logs RE */
    pthread_mutex_lock(&addr[0].logmut); 
    log_game("receive-cards",hand,ownNUMBER,ownNAME,addr[0].filename);
    pthread_mutex_unlock(&addr[0].logmut); 
    
    /* Warns all players who is the first one playing */
    fprintf (stderr, "\n%s is complete! Game may start!\n\n\n>> [%s] is the first one playing.\n\n%s > ",addr[0].room,addr[0].players[addr[0].turn].nickname,ownNAME);
    fflush(stderr);

    while(addr[0].deck_size!=0){    
    /* Waits for receiving the card after the round */
    pthread_mutex_lock(&addr[0].mut); 
    while (read(fd_read,aux,sizeof(aux))<2)
        pthread_cond_wait(&addr[0].cvar, &addr[0].mut);
    pthread_mutex_unlock(&addr[0].mut);    
    
    /* Adds card to hand */
    strcpy(handcards[handsize],aux);
    handsize++;
    format_hand(handcards,handsize,hand);
    fprintf(stderr,"\nReceived Card:%s\n\n%s > ",aux,ownNAME);
    fflush(stderr);

    /* Logs RE */
    pthread_mutex_lock(&addr[0].logmut); 
    log_game("receive-card",hand,ownNUMBER,ownNAME,addr[0].filename);
    pthread_mutex_unlock(&addr[0].logmut);    
}

    return 0;
}

void *dealer_handler(void *arg){
int check=0,i;

    shdata *addr=(shdata*)arg;

        printf("\nWaiting for other players to join\n");

        /* Waits for all players to get in */
        pthread_mutex_lock(&addr[0].mut); 
        while (addr[0].in != addr[0].nplayers)
            pthread_cond_wait(&addr[0].cvar, &addr[0].mut);

        /* Opens all player FIFOS for writing so it can give the cards */
        for(i=0;i<addr[0].in;i++){
                check++;
                fd_write[i]=open(addr[0].players[i].FIFOname,O_WRONLY);
                if(fd_write[i]==-1){
                    printf("Could not open player %d FIFO. Abort\n",i);
                    return 0;
                }
            }
        pthread_mutex_unlock(&addr[0].mut); 


    /* Gives the cards at beginning */
    for(i=0;i<addr[0].in;i++)
        distributing_cards(addr[0].cards,&addr[0].deck_size,addr[0].players[i].FIFOname,i,fd_write[i]);
        
    /* Logs DE*/
    pthread_mutex_lock(&addr[0].logmut); 
    log_game("deal", "-",0,ownNAME,addr[0].filename);
    pthread_mutex_unlock(&addr[0].logmut); 

    /* Decides who the first one playing is */
    srand (time(NULL));
    if(addr[0].nplayers==1)
        addr[0].turn =0;
    else
        addr[0].turn = rand() % addr[0].nplayers;
    

    /* Broadcasts to warn all players that the game is ready to be played.
     * Sets both timers.
    */
    pthread_mutex_lock(&addr[0].mut2);
    addr[0].roundnumber=1;
    addr[0].roundtimer=time(NULL);
    addr[0].timer=time(NULL);
    pthread_cond_broadcast(&addr[0].cvar2);
    pthread_mutex_unlock(&addr[0].mut2); 
    
    /* Loop:
     * Stops giving cards once deck_size=0
    */
    while(1){
        
        /* Waits for all cards to be in the table */
        pthread_mutex_lock(&addr[0].tablemut); 
            while (addr[0].tablein != addr[0].nplayers)
                pthread_cond_wait(&addr[0].ctable, &addr[0].tablemut);
        pthread_mutex_unlock(&addr[0].tablemut); 
        
        /* Resets the lastround string */
        memset(addr[0].lastround,'\0',sizeof(addr[0].lastround));
        format_hand(addr[0].tablecards,addr[0].tablein,addr[0].lastround);
        for(i=0;i<addr[0].roomsize;i++)
            memset(addr[0].tablecards[i],'\0',sizeof(addr[0].tablecards[i]));

        /* Gives one card to all players and warns them */
       if(addr[0].deck_size!=0){
       pthread_mutex_lock(&addr[0].mut);
        for(i=0;i<addr[0].in;i++)
            give_card(addr[0].cards,&addr[0].deck_size,addr[0].players[i].FIFOname,i,fd_write[i]);
        pthread_cond_broadcast(&addr[0].cvar);
        pthread_mutex_unlock(&addr[0].mut); 
        
        /* Logs DE*/
        pthread_mutex_lock(&addr[0].logmut); 
        log_game("deal", "-",0,ownNAME,addr[0].filename);
        pthread_mutex_unlock(&addr[0].logmut); 
            }
        addr[0].tablein=0;
        addr[0].roundnumber+=1;
        addr[0].roundtimer=time(NULL);
    }
    return 0;
}


void change_turn(int nplayers,int *turn, long int *timer){

if((*turn)==(nplayers-1))
    (*turn)=0;
else
    (*turn)++;
*timer=time(NULL);

}

void fill_str(int space, char *str, char *str_to_fill){

int i,j=0,len=strlen(str);
    if(space==1)
        j=1;

    for(i=0;i<len;i++)
    {

        str_to_fill[j+i]=str[i];
    }
}

int log_game(char *action, char *outcome,int pnumber,char *name, char *filename){
  char aux[100]="";
  char number[3]={((char)((int) '0'+pnumber)),'-'};
  char when[21]={[0 ... 19] = ' '};
  char what[18]={[0 ... 17] = ' '};
  char result[44]={[0 ... 43] = ' '};
 char who[21]={[0 ... 19] = ' '};
 

int log_fd=open(filename, O_RDWR | O_APPEND);
int ctr=0;
    /*WHEN */
    fill_str(0, gettime(), when);

    ctr+= write(log_fd,when,sizeof(when)-1);  
    ctr+=write(log_fd,"|",sizeof("|")-1); 
    /*WHO*/
    if(strcmp(action,"deal")==0)
        strcat(aux,"Dealer-");
    else{
        strcat(aux,"Player");
        strcat(aux,number);
    }
    
    strcat(aux,name);
    //printf("aux:%s",aux);
    fill_str(1, aux, who);
    ctr+=write(log_fd,who,sizeof(who)-1); 
   ctr+=write(log_fd,"|",sizeof("|")-1); 

    /*WHAT*/
    fill_str(1, action, what);
    ctr+=write(log_fd,what,sizeof(what)-1); 
    ctr+=write(log_fd,"|",sizeof("|")-1); 
    
    /*RESULT*/
    fill_str(1, outcome, result);
    ctr+=write(log_fd,result,sizeof(result)-1); 
   ctr+= write(log_fd,"\n",sizeof("\n")-1); 

    close(log_fd);
    if(ctr<=0)
        return errno;
    return 0;
  
}



char *gettime(){
 time_t rawtime;
  struct tm * timeinfo;

   static char result[100];
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

    sprintf(result, "%d-%.2d-%.2d %.2d:%.2d:%.2d \n",
    1900 + timeinfo->tm_year,
    timeinfo->tm_mon,
    timeinfo->tm_mday,
    timeinfo->tm_hour,
    timeinfo->tm_min,
    timeinfo->tm_sec
    );
    
    return result;

}