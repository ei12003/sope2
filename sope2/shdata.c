#include "shdata.h"




void initalize_data(shdata *data, int room_size){
    
    int log_fd,i;
    char buf[]="when                | who                | what            | result\n";
    log_fd=open("shname.log", O_RDWR | O_CREAT, 0666);
    write(log_fd,buf,sizeof(buf)-1);
    close(log_fd);
    for(i=0;i<6;i++)
        memset(data[0].tablecards[i],'\0',sizeof(data[0].tablecards[i]));
    data[0].deck_size=52;
    data[0].nplayers=room_size;
    data[0].in=0;
    data[0].tablein=0;
    data[0].turn=-1;
    data[0].roundnumber=0;
    data[0].dealer=0;
    data[0].changed=0;
}


void add_player_to_shdata(shdata *data,char* name,int *ownNUMBER){
    data[0].in++;
    char fifo[40] = "FIFO";
    strcat(fifo,name);
    strcpy(data[0].players[data[0].in-1].nickname,name);
    strcpy(data[0].players[data[0].in-1].FIFOname,fifo);
    data[0].players[data[0].in-1].number=data[0].in-1;
    (*ownNUMBER)=data[0].in-1;
}


void init_sync_objects_in_shared_memory(shdata *data)
{
pthread_mutexattr_t mattr;
pthread_mutexattr_init(&mattr);
pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
pthread_mutex_init(&data[0].mut, &mattr);
pthread_mutex_init(&data[0].mut2, &mattr);
pthread_mutex_init(&data[0].tablemut, &mattr);

pthread_condattr_t cattr;
pthread_condattr_init(&cattr);
pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
pthread_cond_init(&data[0].cvar, &cattr);
pthread_cond_init(&data[0].cvar2, &cattr);
pthread_cond_init(&data[0].ctable, &cattr);


} 


int cleanall(shdata *addr, int shmid,int *fd_write){
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

int create_fifo(shdata *addr,char *ownFIFO,int *fd_read){
    int fifo;
    char *path=addr[0].players[addr[0].in-1].FIFOname;
    fifo=mkfifo(path, 0666); 
    if(fifo==-1)
        return -1;
    strcpy(ownFIFO,path);
    printf("abrir %s",path);
    (*fd_read)=open(path,O_RDONLY|O_NONBLOCK);        
    printf("aberto");
    
    return 0;

}

int play_card(char* card,char handcards[6][4],char *hand_str,int *size,shdata *addr)
{
int i,aux,j;
char tmp[5];


for(i=0;i<*size;i++){
    if(strcmp(handcards[i],card)==0)
    {   
        printf("\ncard to play[%d][%d]:%s",*size-i,i,handcards[i]);   
        for(j=0;j<(*size)-i;j++){
            printf("\nin");
            strcpy(handcards[i+j],handcards[i+j+1]);

        }
        (*size)--;
        
        format_hand(handcards,*size,hand_str);

        
        strcpy(addr[0].tablecards[(++(addr[0].tablein))-1],card);
        printf("\ntab[%d]:%s\n##|\n",(addr[0].tablein),addr[0].tablecards[0]);
        if(addr[0].nplayers==addr[0].tablein){
                pthread_mutex_lock(&addr[0].tablemut);
                pthread_cond_signal(&addr[0].ctable);
                pthread_mutex_unlock(&addr[0].tablemut); 
        }

        
        return 0;
    }
}

    return -1;

}

shdata *joinroom(char *name, char *room, int room_size, int *shmid,int *ownNUMBER,char *ownFIFO, int *fd_read, int *isDealer){
    
    shdata *addr;
    key_t key;
    int fifo;
    char fifoplayer0[40]="FIFO";
    key=ftok(room,0);
    *shmid = shmget(key, 0,0);

    if(*shmid==-1){
        printf("Creating\n");
        *isDealer=1;
        *shmid=shmget(key, sizeof(shdata), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);    
        addr=(shdata*)shmat(*shmid,0,0);
        initalize_data(addr,room_size);
        strcat(fifoplayer0,name);

        mkfifo(fifoplayer0, 0666); 
        (*fd_read)=open(fifoplayer0,O_RDONLY|O_NONBLOCK);        
    }

    else{
        *isDealer=0;
        addr=(shdata*)shmat(*shmid,0,0);
        printf("Joining%d\n",addr[0].in);
        if(addr[0].in==addr[0].nplayers){        
            addr[0].failed=1;
            return addr;
        }
    }
    printf("\nacrescentar player");
    add_player_to_shdata(addr,name,ownNUMBER);
    /*##########
        Setting up FIFOname
            ##########*/
        printf("\ncreatefifo");
     if(*isDealer==0)
           fifo=create_fifo(addr,ownFIFO,fd_read);

   if(fifo==-1){
        addr[0].failed=2;
        return addr;
    }


    return addr;
}