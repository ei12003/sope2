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
/*void print_shdata(shdata data){
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
}*/

void add_player_to_shdata(shdata *data,char* name, int player_number){
    data[0].in++;
    if(player_number==0)
        data[0].players=(player_entry*) malloc (sizeof(player_entry)*data[0].nplayers);
        
    player_entry player;
    player.nickname=name;
    player.FIFOname=name;
    
    data[0].players[data[0].in-1]=player;
}

shdata *joinroom(char *name, char *room, int room_size, int *shmid){
    
    shdata *addr;
    key_t key;
    key=ftok(room,0);
    *shmid = shmget(key, 0,0);

    if(*shmid==-1){
        printf("Creating\n");
        *shmid=shmget(key, sizeof(shdata), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
        addr=(shdata*)shmat(*shmid,0,0);
        addr[0].nplayers=room_size;
        add_player_to_shdata(addr,name,0);
        printf("addr nick p0: %s",addr[0].players[0].nickname); 
    }
    else{
        printf("Joining\n");
        addr=(shdata*)shmat(*shmid,0,0);
        
    }
    

    printf("shmid=%d",*shmid);
    return addr;
}


int main(int argc, char *argv[], char *envp[])
{

char *player_name,*room;
int room_size;
shdata *addr;
int shmid=0;;
//tpc Manuel mesa1 4
    if(argc!=4){
        printf("Invalid number of arguments\n");
        return -1;
    }

    player_name=argv[1];
    printf("\nPlayer name: %s\n",player_name);
    room=argv[2];
    room_size=atoi(argv[3]);
    printf("Room[%d]: %s\n",room_size,room);
    
    addr=joinroom(player_name,room,room_size,&shmid);
    printf("aAAAAAnick p0: %s",addr[0].players[0].nickname); 

  //  addr=(shdata*)shmat(shmid,0,0);
//    printf("addr nick p0: %s",addr[0].players[0].nickname);
    //printf("addr nick p0: %d",shmid);
    //print_shdata(addr[0]);





return 0;





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


pthread_create(&tid, NULL, thr_func, &k);
pthread_join(tid, &r);

//printf("|%c|pt2[2]=%c",(char)50,pt1[2]);
shmdt(pt1);
shmctl(shmid, IPC_RMID, NULL);
free(r);
exit(0);*/
}

