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
pthread_cond_t cvar=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER; 




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
    //if(isDealer!=1)
      //  pthread_cond_signal(&cvar);


    if(isDealer==1)
        init_deck(addr);
    
    print_shdata(addr[0]);


    char ch;
    if(isDealer==1){
        do{
            scanf("%c",&ch);
        }while(ch!='e');
        
        cleanall(addr,shmid);
    }/*
if(isDealer==1){
        pthread_mutex_lock(&mut); 
        while (addr[0].in != addr[0].nplayers){
            pthread_cond_wait(&cvar, &mut); 
            printf("gonnacheck");
            }
        pthread_mutex_lock(&mut); 
}*/

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


shuffle_deck(addr[0].cards);

}


void shuffle_deck(card *cards){
card tmp;
int i,j;
srand(time(NULL));
for(i=0;i<52;i++){
    j = rand() % 52; 
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
