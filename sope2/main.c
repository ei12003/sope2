#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* OI */

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




int main(void)
{
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
exit(0);
}

