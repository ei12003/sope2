#include "main.h"


int isDealer;
int fd_write[10];
int fd_read,ownNUMBER;
char hand[150];
char ownFIFO[40], ownNAME[40];

int play_card(char *hand, char* card);
int log_game(char *what, char *result,int pnumber,char *name);
void fill_str(int spaces, char *str, char *str_to_fill);

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
void read_hand(char* hand,char handcards[6][4]){
int len=strlen(hand);
int i,j,s,k=0;
char rank[3]={0};
char suit;


//Ac-5c-8c-Jc/Ah-2h/6d-10d-Kd/3s-5s-4s-7s
//read_hand("Jc/2h/10d-Kd/3s-5s");
for(i=0;i<len;i++){
    if(hand[i]!='/' && hand[i]!='-'){
        rank[0]=hand[i];
    
        if(hand[i+1]=='0'){
            i++;
            rank[1]=hand[i];
        }
        
        i++;
        suit=hand[i];
        
        
        for(j=0;j<strlen(rank);j++){
            handcards[k][j]=rank[j];
        }
        handcards[k][j]=suit;
        
        k+=1;
        memset(rank, '\0',sizeof(rank));
        }
    }
    for(i=0;i<6;i++)
        printf("card[%d]:%s\n",i,handcards[i]);
    //printf("\nstr:%c",handcards[0][strlen(handcards[0])-1]);
}


void format_hand(char handcards[6][4],int size,char *hand_str){
char str;
int i,j;

    char suit[]={'c','d','h','s'};
    int check;
    int c;

    

    for(i=0;i<4;i++){
        for(j=0;j<size;j++){
            if(handcards[j][strlen(handcards[j])-1]==suit[i]){
                
                check=1;
                strcat(hand_str,handcards[j]);
                strcat(hand_str,"-");
            }

        }
if(check){
        hand_str[strlen(hand_str)-1]='\0';
        strcat(hand_str,"/");
        check=0;
        }
    }
    hand_str[strlen(hand_str)-1]='\0';
    
printf("\nstr:%s",hand_str);

}

int main(int argc, char *argv[], char *envp[])
{
        

shdata *addr;
pthread_t tidp_gp,tidp_kbd,tidd;
int shmid=0;
char hand_str[200]="";
char handcards[6][4]={0};
char **var1;
char *var2;
read_hand("Jc/2h/10d-Kd/3s-5s",handcards);

format_hand(handcards,6,hand_str);

 printf("\n\n###\n%s",hand_str);
/*
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

    //char buf[]="when    |who    |what   |result";

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

int play_card(char *hand, char *card){

int loc=strstr(hand,card);
int len=strlen(card);
if(loc==NULL)
    return -1;
return 0;
    







}
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
                printf("[Hand: %s]\n",hand);
            }
            else if(strcmp(ch,"turn")==0){
                printf("[Turn: %d]\n",addr[0].turn);
                aux[0]=((char)((int) '0'+addr[0].turn));
                printf("\nturnbytes:%d|\n",log_game("turn",aux,ownNUMBER,ownNAME));
            }
           /* else if(strstr(ch,"play:")!=NULL)
                if(addr[0].turn!=ownNUMBER)
                    printf("It's not your turn to play!");
                else{
                    c=sscanf(ch,"play:%s",card);
                    c=play_card(hand,card);
                    if(c==-1)
                        printf("You don't have that card");
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
    
    printf("bytesread %d\n",read(fd_read,hand,sizeof(hand)));
    printf("\nrecebytes:%d\n",log_game("receive-cards",hand,ownNUMBER,ownNAME));
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
    for(i=0;i<addr[0].in;i++) {
        distributing_cards(addr[0].cards,&addr[0].deck_size,addr[0].players[i].FIFOname,i);
        
    }
    printf("\ndisbytes:%d\n",log_game("deal", "-",0,ownNAME));

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
int check=0;
for(j=0;j<HANDLIMIT;j++){
        hand[j]=cards[j]; 
        printf("[%s %c]",hand[j].rank,hand[j].suit);
    }
    //hand_str[0]='\0';
    char suit[]={'c','d','h','s'};
    int size;
    for(i=0;i<4;i++){
        for(j=0;j<HANDLIMIT;j++){
            if(hand[j].suit==suit[i]){
                check=1;
                strcat(hand_str,hand[j].rank);
                size=strlen(hand_str);
                hand_str[size]=hand[j].suit;
                //hand_str[size+1]='\0';
                strcat(hand_str,"-");
            }
        }
if(check){
        hand_str[strlen(hand_str)-1]='\0';
        strcat(hand_str,"/");
        check=0;
        }
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
    
    int log_fd;

    char buf[]="when                | who                | what            | result\n";
    log_fd=open("shname.log", O_RDWR | O_CREAT, 0666);
    write(log_fd,buf,sizeof(buf)-1);
    close(log_fd);

    data[0].deck_size=52;
    data[0].nplayers=room_size;
    data[0].in=0;
    data[0].turn=-1;
    data[0].roundnumber=0;
    data[0].dealer=0;
}
