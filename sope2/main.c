#include "main.h"

/* OI 

int oi;
void *thr_func(void *arg){

int shmid;
int *pt2;

key_t key;

key=ftok("proc1",0);

shmid = shmget(key, 0,0);
pt2=(int*)shmat(shmid,0,0);


pt2 = (int *) shmat(shmid, 0, 0);


printf("pt2[2]=%s",pt2[0]);

shmdt(pt2);

return NULL;
}
*/
int isDealer;


void init_sync_objects_in_shared_memory(shdata *data)
{
pthread_mutexattr_t mattr;
pthread_mutexattr_init(&mattr);
pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
printf("pthread_mutex_init=%d", pthread_mutex_init(&data[0].mut, &mattr));

pthread_condattr_t cattr;
pthread_condattr_init(&cattr);
pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
printf("pthread_cond_init=%d", pthread_cond_init(&data[0].cvar, &cattr));


} 

void remove_cards(card *deck,int *deck_size,int number){
int i,j;
for(j=0;j<number;j++){
    for(i=0;i<(*deck_size)-1;i++)
        deck[i]=deck[i+1];
    (*deck_size)--;
}
}

int distributing_cards(shdata *addr){
    int i,j,fd;
    char *fifo="fifotest";
    card hand[HANDLIMIT];
    char hand_str[150]="";

    for(i=0;i<addr[0].in;i++)
        fifo=addr[0].players[i].FIFOname;

    for(j=0;j<HANDLIMIT;j++){
        hand[j]=addr[0].cards[j]; 
        printf("[%s %c]",hand[j].rank,hand[j].suit);
    }

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
    printf("[hand: %s]",hand_str);    
    int fd_tmp=open(fifo,O_RDONLY|O_NONBLOCK);
    fd=open(fifo,O_WRONLY);
    if(fd==-1){
        printf("ups2");
        return -1;
    }
    write(fd,hand_str, sizeof(hand_str));
        
    if(close(fd)!=0)
        printf("cant close fd\n");
    if(close(fd_tmp)!=0)
        printf("cant close fd_tmp\n");
    char ch;
    scanf("%c",&ch);

    remove_cards(addr[0].cards,&addr[0].deck_size,HANDLIMIT);
    return 0;
}
//c-5c-8c-Jc/Ah-2h/6d-10d-Kd/3s-5s-4s-7s--------------------------------------------------------------
//write(fd, const void *buf, sizeof()); 


int main(int argc, char *argv[], char *envp[])
{

shdata *addr;
int shmid=0;;

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
    if(isDealer!=1){
        printf("\noi1\n");
        pthread_mutex_lock(&addr[0].mut);
        printf("oi2");
        pthread_cond_signal(&addr[0].cvar);
        pthread_mutex_unlock(&addr[0].mut); 

    }

    if(isDealer==1){
        init_deck(addr);
        init_sync_objects_in_shared_memory(addr);
    
        printf("\nWaiting for other players to join\n");
        pthread_mutex_lock(&addr[0].mut); 
        while (addr[0].in != addr[0].nplayers)
            pthread_cond_wait(&addr[0].cvar, &addr[0].mut);
        pthread_mutex_unlock(&addr[0].mut); 

        printf("\nDistributing cards\n");
        distributing_cards(addr);

        char ch;
        do{
            printf("prompt > ");
            scanf("%c",&ch);

        }while(ch!='e');
        
        cleanall(addr,shmid);
    }

return 0;



}


shdata *joinroom(char *name, char *room, int room_size, int *shmid){
    
    shdata *addr;
    key_t key;
    int fifo;
    key=ftok(room,0);
    *shmid = shmget(key, 0,0);

    if(*shmid==-1){
        printf("Creating\n");
        isDealer=1;
        *shmid=shmget(key, sizeof(shdata), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);    
        addr=(shdata*)shmat(*shmid,0,0);
        initalize_data(addr,room_size);
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
    
    add_player_to_shdata(addr,name);
    /*##########
        Setting up FIFOname
            ##########*/
        fifo=create_fifo(addr);
   if(fifo==-1){
        addr[0].failed=2;


        return addr;
    }


    return addr;
}

void init_deck(shdata *addr){


srand (time(NULL));
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
    //open(path,O_NONBLOCK);
    return 0;

}

int cleanall(shdata *addr, int shmid){
    int i;
    int size=addr[0].in;
    
    for(i=0;i<size;i++){
      //  close(addr[0].players[i].FIFOname);
        unlink(addr[0].players[i].FIFOname);
    }
    shmdt(addr);
    shmctl(shmid, IPC_RMID, NULL); 
    return 0;
}

void print_shdata(shdata data){
int i;
printf("\n\nSHDATA---------");
printf("\nnplayers: %d",data.nplayers);
printf("\nin: %d",data.in);
printf("\nturn: %d",data.turn);
printf("\nroundnumber: %d",data.roundnumber);
printf("\ndealer: %d",data.dealer);

printf("\n# Player Entry");
for(i=0;i<data.in;i++)
    printf("\n%d | %s | %s",data.players[i].number,data.players[i].nickname,data.players[i].FIFOname);

printf("\n##### Deck\n");
for(i=0;i<52;i++)
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
}
void initalize_data(shdata *data, int room_size){
    data[0].deck_size=52;
    data[0].nplayers=room_size;
    data[0].in=0;
    data[0].turn=0;
    data[0].roundnumber=0;
    data[0].dealer=0;
}


    /*
pthread_t tid;
int k = 10;
oi=0;
void *r;
int shmid;
int *pt1;
key_t key;
char *oi="olá";


key=ftok("proc1",0);

shmid = shmget(key, 1024, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
pt1 = (int *) shmat(shmid, 0, 0);
pt1[0] =oi;

pthread_t tid;

pthread_create(&tid, NULL, thr_func, &k);
pthread_join(tid, &r);

//printf("|%c|pt2[2]=%c",(char)50,pt1[2]);
shmdt(pt1);
shmctl(shmid, IPC_RMID, NULL);
free(r);
exit(0);*/
