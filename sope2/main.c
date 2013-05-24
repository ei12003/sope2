#include "main.h"


int isDealer;
int fd_write[10];
int fd_read,ownNUMBER;
char ownFIFO[40], ownNAME[40];
int log_fd;

int main(int argc, char *argv[], char *envp[])
{
        

shdata *addr;
pthread_t tidp_gp,tidp_kbd,tidd;
int shmid=0;




 if(argc!=4){
        printf("Invalid number of arguments\n");
        return -1;
    }

    //#####
    //  Setting up Memoryshare 
    //     #####

    addr=joinroom(argv[1],argv[2],atoi(argv[3]),&shmid);

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

    pthread_create(&tidp_gp, NULL, player_gameplay_handler, addr);
    pthread_create(&tidp_kbd, NULL, player_kbd_handler, addr);
    pthread_join(tidp_kbd, NULL);

    close(fd_read);
    
    if(isDealer==1)
       cleanall(addr,shmid);
    else
        shmdt(addr);

return 0;
// */

}

/* ##################
        Functions
   ################## */

void log(char *what,char str,int pnumber,char *name,shdata *data){
  char buf[];
  char time[]="time_to_implement	|",player,what,result;
  
  strcat(buf,time);
  strcat(buf,"Player");
  // numberstrcat(buf,
  strcat(buf,"-");
  strcat(buf,name);
  strcat(buf,"	|");
  
  write(log_fd,buf,sizeof(log_fd));  
}


void *player_kbd_handler(void *arg){
        shdata *addr=(shdata*)arg;
        char ch[100];
        char hand[150];
        int rank;
        char suit='W';
        int c;
        
            while(1){
           printf("promptPLAYER > ");
            scanf("%s",ch);
            
            if(strcmp(ch,"show")==0){
                read(fd_read,hand,sizeof(hand));
                printf("[Cards: %s]\n",hand);
            }
            else if(strcmp(ch,"turn")==0)
                printf("[Turn: %d]\n",addr[0].turn);
            else if(strstr(ch,"play:")!=0)
                if(addr[0].turn!=ownNUMBER)
                    printf("It's not your turn to play!");
                else{
                    c=sscanf(ch,"play:%d%c",&rank,&suit);
		    
		    
		    
		}
              /* PLAY & CHECK CARD */
            else if(strcmp(ch,"exit")==0)
                break;
	    else 
	    {
	     printf("Supported commands:\nshow; play:RS; turn; exit");
	      
	    }
            printf("\n");
                
        }

    return 0;
}

void *player_gameplay_handler(void *arg){
    shdata *addr=(shdata*)arg;
    pthread_mutex_lock(&addr[0].mut);
    pthread_cond_signal(&addr[0].cvar);
    pthread_mutex_unlock(&addr[0].mut); 

    printf("\nisPlayer: %s ",addr[0].players[1].nickname);
    pthread_mutex_lock(&addr[0].mut); 
    while (addr[0].turn == -1)
        pthread_cond_wait(&addr[0].cvar, &addr[0].mut);
    pthread_mutex_unlock(&addr[0].mut); 
    //puts("\nOIIII");
  //  printf("\n[Dealer%c] %s is the one playing.\npromptPLAYER > ",addr[0].turn,addr[0].players[addr[0].turn].nickname);
    
    if(isDealer==0){
    fprintf (stderr, "\n[Dealer] %s is the one playing.\npromptPLAYER > ",addr[0].players[addr[0].turn].nickname);
    fflush(stderr);
}
    return 0;
}

void *dealer_handler(void *arg){
int check=0,i;

    shdata *addr=(shdata*)arg;
    printf("\nisDealer: %s ",addr[0].players[0].nickname);

        init_deck(addr);
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


//        int distributing_cards(card *cards,int deck_size,char fifo[]){
    for(i=0;i<addr[0].in;i++) 
        distributing_cards(addr[0].cards,&addr[0].deck_size,addr[0].players[i].FIFOname,i);

    print_shdata(addr[0]);

    pthread_mutex_lock(&addr[0].mut);
    srand (time(NULL));
    if(addr[0].nplayers==1)
        addr[0].turn =0;
    else
        addr[0].turn = rand() % addr[0].nplayers;
    
    pthread_cond_broadcast(&addr[0].cvar);
    pthread_mutex_unlock(&addr[0].mut); 
    fprintf (stderr, "\n[Dealer] %s is the one playing.\npromptPLAYER > ",addr[0].players[addr[0].turn].nickname);
    fflush(stderr);

    
    return 0;
}


void init_sync_objects_in_shared_memory(shdata *data)
{
pthread_mutexattr_t mattr;
pthread_mutexattr_init(&mattr);
pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
pthread_mutex_init(&data[0].mut, &mattr);

pthread_condattr_t cattr;
pthread_condattr_init(&cattr);
pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
pthread_cond_init(&data[0].cvar, &cattr);


} 

void remove_cards(card *deck,int *deck_size,int number){
int i,j;
for(j=0;j<number;j++){
    for(i=0;i<(*deck_size)-1;i++)
        deck[i]=deck[i+1];
    (*deck_size)--;
}
}

int distributing_cards(card *cards,int *deck_size,char *fifo,int player_number){

    int i,j;
    card hand[HANDLIMIT];
    char hand_str[150]="";

printf("------>PLAYER%s",fifo);
for(j=0;j<HANDLIMIT;j++){
        hand[j]=cards[j]; 
        printf("[%s %c]",hand[j].rank,hand[j].suit);
    }
    hand_str[0]='\0';
    char suit[]={'c','d','h','s'};
    int size;
    for(i=0;i<4;i++){
        for(j=0;j<HANDLIMIT;j++){
            if(hand[j].suit==suit[i]){
                strcat(hand_str,hand[j].rank);
                size=strlen(hand_str);
                hand_str[size]=hand[j].suit;
                hand_str[size+1]='\0';
                strcat(hand_str,"-");
            }
        }

        hand_str[strlen(hand_str)-1]='\0';
        strcat(hand_str,"/");
    }
    hand_str[strlen(hand_str)-1]='\0';
    
    printf("\nplayer_number:%d\nhand_str:%s",player_number,hand_str);
    printf("!!printing %d",(int)write(fd_write[player_number],hand_str, sizeof(hand_str)));
    remove_cards(cards,deck_size,HANDLIMIT);
    printf("$$ cards removed%d",*deck_size);
    return 0;
}




shdata *joinroom(char *name, char *room, int room_size, int *shmid){
    
    shdata *addr;
    key_t key;
    int fifo;
    char fifoplayer0[40]="FIFO";
    key=ftok(room,0);
    *shmid = shmget(key, 0,0);

    if(*shmid==-1){
        printf("Creating\n");
        isDealer=1;
        *shmid=shmget(key, sizeof(shdata), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);    
        addr=(shdata*)shmat(*shmid,0,0);
        initalize_data(addr,room_size);
        strcat(fifoplayer0,name);

        mkfifo(fifoplayer0, 0666); 
        fd_read=open(fifoplayer0,O_RDONLY|O_NONBLOCK);        
    }

    else{
        isDealer=0;
        addr=(shdata*)shmat(*shmid,0,0);
        printf("Joining%d\n",addr[0].in);
        if(addr[0].in==addr[0].nplayers){        
            addr[0].failed=1;
            return addr;
        }
    }
    printf("\nacrescentar player");
    add_player_to_shdata(addr,name);
    /*##########
        Setting up FIFOname
            ##########*/
        printf("\ncreatefifo");
     if(isDealer==0)
           fifo=create_fifo(addr);

   if(fifo==-1){
        addr[0].failed=2;
        return addr;
    }


    return addr;
}

void init_deck(shdata *addr){
int i,j;
int ctr=0;
char rank[]={'A','2','3','4','5','6','7','8','9','J','Q','K'};
char suit[]={'c','d','h','s'};
for(i=0;i<12;i++)
{
    for(j=0;j<4;j++){
        addr[0].cards[ctr].rank[0]=rank[i];        
        addr[0].cards[ctr].rank[1]='\0';     
        addr[0].cards[ctr].suit=suit[j];
        ctr++;
    }
}

for(j=0;j<4;j++){
    addr[0].cards[ctr].rank[0]='1';        
    addr[0].cards[ctr].rank[1]='0';        
    addr[0].cards[ctr].rank[2]='\0'; 
    addr[0].cards[ctr].suit=suit[j];   
    ctr++;
}


shuffle_deck(addr[0].cards,addr[0].deck_size);

}


void shuffle_deck(card *cards, int deck_size){
card tmp;
int i,j;
srand(time(NULL));
for(i=0;i<deck_size;i++){
    j = rand() % deck_size; 
    tmp=cards[i];
    cards[i]=cards[j];
    cards[j]=tmp;
}
}
int create_fifo(shdata *addr){
    int fifo;
    char *path=addr[0].players[addr[0].in-1].FIFOname;
    fifo=mkfifo(path, 0666); 
    if(fifo==-1)
        return -1;
    strcpy(ownFIFO,path);
    printf("abrir %s",path);
    fd_read=open(path,O_RDONLY|O_NONBLOCK);        
    printf("aberto");
    
    return 0;

}

int cleanall(shdata *addr, int shmid){
    int i;
    int size=addr[0].in;
    
    for(i=0;i<size;i++){
        close(fd_write[i]);
        unlink(addr[0].players[i].FIFOname);
    }
    shmdt(addr);
    shmctl(shmid, IPC_RMID, NULL); 
    close(log_fd);
    return 0;
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

void add_player_to_shdata(shdata *data,char* name){
    data[0].in++;
    char fifo[40] = "FIFO";
    strcat(fifo,name);
    strcpy(data[0].players[data[0].in-1].nickname,name);
    strcpy(data[0].players[data[0].in-1].FIFOname,fifo);
    data[0].players[data[0].in-1].number=data[0].in-1;
    ownNUMBER=data[0].in-1;
}
void initalize_data(shdata *data, int room_size){
    
    char buf[]="when	|who	|what	|result";

    log_fd=open("shname.log", O_RDWR | O_CREAT, 0666);
    write(log_fd,buf,sizeof(log_fd));
    
    data[0].deck_size=52;
    data[0].nplayers=room_size;
    data[0].in=0;
    data[0].turn=-1;
    data[0].roundnumber=0;
    data[0].dealer=0;
}
