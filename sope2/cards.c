
#include "cards.h"
#include <time.h>


void init_deck(card *cards, int *deck_size){
int i,j;
int ctr=0;
char rank[]={'A','2','3','4','5','6','7','8','9','J','Q','K'};
char suit[]={'c','d','h','s'};
for(i=0;i<12;i++)
{
    for(j=0;j<4;j++){
        cards[ctr].rank[0]=rank[i];        
        cards[ctr].rank[1]='\0';     
        cards[ctr].suit=suit[j];
        ctr++;
    }
}

for(j=0;j<4;j++){
    cards[ctr].rank[0]='1';        
    cards[ctr].rank[1]='0';        
    cards[ctr].rank[2]='\0'; 
    cards[ctr].suit=suit[j];   
    ctr++;
}


shuffle_deck(cards,*deck_size);

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
    int check=0;
    int c;
    
    memset(hand_str,'\0',sizeof(hand_str));
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
}

void remove_cards(card *deck,int *deck_size,int number){
int i,j;
for(j=0;j<number;j++){
    for(i=0;i<(*deck_size)-1;i++)
        deck[i]=deck[i+1];
    (*deck_size)--;
}
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

int give_card(card *cards,int *deck_size,char *fifo,int player_number, int fd){
        char hand_str[5]="";
        card card;
        card=cards[0];
        remove_cards(cards,deck_size,1);
        strcat(hand_str,card.rank);
        hand_str[strlen(hand_str)]=card.suit;
        hand_str[strlen(hand_str)]='\0';
        
        write(fd,hand_str, sizeof(hand_str));
}


int distributing_cards(card *cards,int *deck_size,char *fifo,int player_number, int fd){

    int i,j;
    card hand[HANDLIMIT];
    char hand_str[150]="";

    int check=0;
for(j=0;j<HANDLIMIT;j++){
        hand[j]=cards[j]; 
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
    
    //printf("\nplayer_number:%d\nhand_str:%s",player_number,hand_str);
    /*printf("!!printing %d",(int)*/write(fd,hand_str, sizeof(hand_str));
    remove_cards(cards,deck_size,HANDLIMIT);

    
    //printf("$$ cards removed%d",*deck_size);
    return 0;
}
