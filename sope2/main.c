#include "main.h"
/*

when                | who                | what            | result
2013-04-25 22:56:21 | Dealer-oi          | deal            | -                 $
2013-04-25 22:56:21 | Player0-oi         | receive-cards   | 8d/5h-8h-Jh/Qs-8s $
2013-04-25 22:56:23 | Player0-oi         | hand            | 8d/5h-8h-Jh/Qs-8s $
2013-04-25 22:56:27 | Player0-oi         | play            | 8d                $
2013-04-25 22:56:30 | Player0-oi         | hand            | Jc/5h-8h-Jh/Qs    $
2013-04-25 22:56:45 | Player0-oi         | play            | Jc                $
2013-04-25 22:56:46 | Player0-oi         | hand            | 10c/5h-8h-Jh/Qs   $


VER AQUELE 8S 8D
METER LOG DE DEAL



*/
int isDealer;
int fd_write[10];
int fd_read,ownNUMBER;
char hand[150];
char ownFIFO[40], ownNAME[40];
char handcards[6][4]={{0}};
int handsize=6;
void change_turn(int nplayers,int *turn){

if((*turn)==(nplayers-1))
    (*turn)=0;
else
    (*turn)++;
}

int main(int argc, char *argv[], char *envp[])
{
        

shdata *addr;
pthread_t tidp_gt,tidp_gp,tidp_kbd,tidd;
int shmid=0;

/*shdata data[1];
char card1[]="8d/5h-8h-Jh/Qs-8s";
char card2[]="5c-9c-4c/2h-6h/9s";
char handcards[6][4]={{0}};
read_hand(card1,handcards);
format_hand(handcards,handsize,hand);
printf("hand:%s\n",hand);
printf("play:%d\n",play_card("8d",handcards,hand,&handsize,data));

printf("hand:%s\n",hand);*/


 if(argc!=4){
        printf("Invalid number of arguments\n");
        return -1;
    }

    //#####
    //  Setting up Memoryshare 
    //     #####

    addr=joinroom(argv[1],argv[2],atoi(argv[3]),&shmid,&ownNUMBER,ownFIFO,&fd_read,&isDealer);

    if(addr[0].failed==1){
        printf("\n##########\nRoom \"%s\" is full.\n", argv[2]);
        return -1;
    }

    else if(addr[0].failed==2){
        printf("\n##########\nCouldn't create FIFO pipe.\n");
        return -2;
    }
    else
        printf("sucessful joining");

    strcpy(ownNAME,argv[1]);
    
    if(isDealer==1)
        pthread_create(&tidd, NULL, dealer_handler, addr);        

    pthread_create(&tidp_gt, NULL, player_turn_wait, addr);
    pthread_create(&tidp_gp, NULL, player_gameplay_handler, addr);
    pthread_create(&tidp_kbd, NULL, player_kbd_handler, addr);
    pthread_join(tidp_kbd, NULL);

    close(fd_read);
    
    if(isDealer==1)
       cleanall(addr,shmid,fd_write);
    else
        shmdt(addr);

return 0;
// */

}

/* ##################
        Functions
   ################## */

    //char buf[]="when    |who    |what   |result";




void *player_kbd_handler(void *arg){
        shdata *addr=(shdata*)arg;
        char ch[100];
        
        int rank;
        char suit='W';
        char aux[3];
        char card[4];
        int c;
        
            while(1){
           printf("promptPLAYER > ");
            scanf("%s",ch);
            
            if(strcmp(ch,"hand")==0){
                printf("\nhandbytes:%d|\n",log_game("hand", hand,ownNUMBER,ownNAME));
              //  format_hand(handcards,handsize,hand);
                printf("[Hand: %s]\n",hand);
            }
            else if(strcmp(ch,"turn")==0){
                printf("[Turn: %d]\n",addr[0].turn);
                aux[0]=((char)((int) '0'+addr[0].turn));
                printf("\nturnbytes:%d|\n",log_game("turn",aux,ownNUMBER,ownNAME));
            }
            else if(strstr(ch,"play:")!=NULL){
                if(addr[0].turn!=ownNUMBER)
                    printf("It's not your turn to play!");
                else{
                    c=sscanf(ch,"play:%s",card);

                    if(strcmp(card,"d0")==0){
                      /*  addr[0].deck_size=0;
                        pthread_mutex_lock(&addr[0].tablemut);
                        pthread_cond_signal(&addr[0].ctable);
                        pthread_mutex_unlock(&addr[0].tablemut); 
                        return ;*/

    pthread_mutex_lock(&addr[0].mut2);
    
       fprintf (stderr, "\nxiii[%d]",addr[0].roundnumber);
    fflush(stderr);
    pthread_cond_signal(&addr[0].cvar2);
    pthread_mutex_unlock(&addr[0].mut2); 

                    }

                    c=play_card(card,handcards,hand,&handsize,addr);
                    //int play_card(char* card,char handcards[6][4],char *hand_str,int *size,char tablecards[9][4],int *tablein);
                    if(c==-1)
                        printf("You don't have that card\n");
                    else{
                        printf("\nplaybytes:%d|\n",log_game("play",card,ownNUMBER,ownNAME));
                        change_turn(addr[0].nplayers,&addr[0].turn);
                        addr[0].changed=1;
                        pthread_mutex_lock(&addr[0].mut);
                        pthread_cond_broadcast(&addr[0].cvar);
                        pthread_mutex_unlock(&addr[0].mut); 
                     }
                    
                }
		    }
              /* PLAY & CHECK CARD */
            else if(strcmp(ch,"exit")==0)
                break;
	    else 
	    {
	     printf("Supported commands:\nhand; play:RS; turn; exit");
	      
	    }
            printf("\n");
                
        }

    return 0;
}


void *player_turn_wait(void *arg){
  shdata *addr=(shdata*)arg;
while(addr[0].turn!=-2){
    pthread_mutex_lock(&addr[0].mut); 
    while (addr[0].turn != ownNUMBER || addr[0].changed==0)
        pthread_cond_timedwait(&addr[0].cvar, &addr[0].mut,100);
    pthread_mutex_unlock(&addr[0].mut); 
    addr[0].changed=0;

    
    if(isDealer)
    fprintf (stderr, "\nIt's your turn!%d\npromptPLAYER > ",ownNUMBER);
    else
        fprintf (stderr, "\nIt's your turn!%d\npromptPLAYER > ",ownNUMBER);
    fflush(stderr);
}
}

void *player_gameplay_handler(void *arg){
    shdata *addr=(shdata*)arg;
    char aux[10]="";
    pthread_mutex_lock(&addr[0].mut);
    pthread_cond_signal(&addr[0].cvar);
    pthread_mutex_unlock(&addr[0].mut); 

    printf("\nisPlayer: %s ",addr[0].players[1].nickname);
    

    pthread_mutex_lock(&addr[0].mut2); 
    while (addr[0].roundnumber == 0){
        fprintf (stderr, "\n123456789");
    fflush(stderr);
        pthread_cond_wait(&addr[0].cvar2, &addr[0].mut2);
      
}

    fprintf (stderr, "##############out\n");
    fflush(stderr);
    pthread_mutex_unlock(&addr[0].mut2); 
    

    printf("bytesread %d\n",read(fd_read,hand,sizeof(hand)));
    fprintf (stderr, "##############oi|%s\n",hand);
    fflush(stderr);
    read_hand(hand,handcards);
    printf("\nrecebytes:%d\n",log_game("receive-cards",hand,ownNUMBER,ownNAME));
    //puts("\nOIIII");
  //  printf("\n[Dealer%c] %s is the one playing.\npromptPLAYER > ",addr[0].turn,addr[0].players[addr[0].turn].nickname);
    
    if(isDealer==0){
    fprintf (stderr, "\n[Dealer] %s is the one playing.\npromptPLAYER > ",addr[0].players[addr[0].turn].nickname);
    fflush(stderr);
}
    while(addr[0].deck_size!=0){    
    pthread_mutex_lock(&addr[0].mut); 
    while (read(fd_read,aux,sizeof(aux))<2)
        pthread_cond_wait(&addr[0].cvar, &addr[0].mut);
    pthread_mutex_unlock(&addr[0].mut);    
    printf("\naux:%s\n",aux);
    strcpy(handcards[handsize],aux);
    handsize++;
    format_hand(handcards,handsize,hand);
    pthread_mutex_lock(&addr[0].mut); 
    printf("\nrecebytes:%d\n",log_game("receive-cards",hand,ownNUMBER,ownNAME));
    pthread_mutex_unlock(&addr[0].mut);    
}

    return 0;
}

void *dealer_handler(void *arg){
int check=0,i;

    shdata *addr=(shdata*)arg;
    printf("\nisDealer: %s ",addr[0].players[0].nickname);

        init_deck(addr[0].cards,&addr[0].deck_size);
        init_sync_objects_in_shared_memory(addr);
    
        printf("\nWaiting for other players to join\n");
        pthread_mutex_lock(&addr[0].mut); 
        while (addr[0].in != addr[0].nplayers)
            pthread_cond_wait(&addr[0].cvar, &addr[0].mut);
            
        for(i=0;i<addr[0].in;i++){
                check++;
                printf("abrir ESCRITA%d cond",i);
                fd_write[i]=open(addr[0].players[i].FIFOname,O_WRONLY);
                printf("aberto");
                if(fd_write[i]==-1){
                    printf("UPS2\n");
                    return 0;
                }
            }

        pthread_mutex_unlock(&addr[0].mut); 

        printf("\nDistributing cards\n");



    for(i=0;i<addr[0].in;i++)
        distributing_cards(addr[0].cards,&addr[0].deck_size,addr[0].players[i].FIFOname,i,fd_write[i]);
        
           fprintf (stderr, "\nasdasdsda");
    fflush(stderr);
    printf("\ndisbytes:%d\n",log_game("deal", "-",0,ownNAME));

    //print_shdata(addr[0]);

    srand (time(NULL));
    if(addr[0].nplayers==1)
        addr[0].turn =0;
    else
        addr[0].turn = rand() % addr[0].nplayers;
    

    pthread_mutex_lock(&addr[0].mut2);
    addr[0].roundnumber=1;
       fprintf (stderr, "\nxiii");
    fflush(stderr);
    pthread_cond_broadcast(&addr[0].cvar2);
    pthread_mutex_unlock(&addr[0].mut2); 

    fprintf (stderr, "\n[Dealer] %s is the one playing.\npromptPLAYER > ",addr[0].players[addr[0].turn].nickname);
    fflush(stderr);

    
    
    while(addr[0].deck_size!=0){
        pthread_mutex_lock(&addr[0].tablemut); 
            while (addr[0].tablein != addr[0].nplayers)
                pthread_cond_wait(&addr[0].ctable, &addr[0].tablemut);
        pthread_mutex_unlock(&addr[0].tablemut); 
        
        pthread_mutex_lock(&addr[0].mut);
        for(i=0;i<addr[0].in;i++)
            give_card(addr[0].cards,&addr[0].deck_size,addr[0].players[i].FIFOname,i,fd_write[i]);
        pthread_cond_broadcast(&addr[0].cvar);
        printf("\ndisbytes:%d\n",log_game("deal", "-",0,ownNAME));
        pthread_mutex_unlock(&addr[0].mut); 
        addr[0].tablein=0;
        addr[0].roundnumber+=1;

    }
    printf("\nEND");

    //distributing_cards(addr[0].cards,&addr[0].deck_size,addr[0].players[i].FIFOname,i,fd_write[i]);

    return 0;
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

int log_game(char *action, char *outcome,int pnumber,char *name){
  char aux[100]="";
  char number[3]={((char)((int) '0'+pnumber)),'-'};
  char when[21]={[0 ... 19] = ' '};
  char what[18]={[0 ... 17] = ' '};
  char result[44]={[0 ... 43] = ' '};
 char who[21]={[0 ... 19] = ' '};
 

int log_fd=open("shname.log", O_RDWR | O_APPEND);



 int ctr=0;
//log_game("receive-cards","Ac-5c/2s",0,"Manuel", data[0].log_fd);
    /*19_|_18_|_15_|_42*/
  /* when|who|what|result */


    /*WHENfind time */
    fill_str(0, gettime(), when);

    ctr+=write(log_fd,when,sizeof(when)-1);  
    write(log_fd,"|",sizeof("|")-1); 
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
   ctr+= write(log_fd,"|",sizeof("|")-1); 
    
    /*RESULT*/
    fill_str(1, outcome, result);
    ctr+=write(log_fd,result,sizeof(result)-1); 
    ctr+=write(log_fd,"\n",sizeof("\n")-1); 

    close(log_fd);
    if(ctr<0)
        return errno;
    return ctr;
  
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



void print_shdata(shdata data){
int i;
/*printf("\n\nSHDATA---------");
printf("\nnplayers: %d",data.nplayers);
printf("\nin: %d",data.in);
printf("\nturn: %d",data.turn);
printf("\nroundnumber: %d",data.roundnumber);
printf("\ndealer: %d",data.dealer);*/

printf("\n# Player Entry\n");
for(i=0;i<data.in;i++){
    printf("###############");
    printf("\n%d | %s | %s\n",data.players[i].number,data.players[i].nickname,data.players[i].FIFOname);
}


printf("\n##### Deck\n");
for(i=0;i<data.deck_size;i++)
   printf("|%s %c|",data.cards[i].rank,data.cards[i].suit);
printf("\n##### %d\n",i);



}

