#include <stdio.h>
#include <stdlib.h>
#define MAXLINE 1000
int main(void){
char line[MAXLINE];
FILE *fpin;
if((fpin=popen("./pipe02b","r")) == NULL){
printf("popen error"); exit(1);}
for( ; ; )
{ fputs("prompt > ",stdout); fflush(stdout);
if(fgets(line,MAXLINE,fpin) == NULL)
break;
if(fputs(line,stdout) == EOF)
{ fprintf(stderr,"fputs error"); exit(1);}
}
if(pclose(fpin)==-1){fprintf(stderr,"pclose error"); exit(1);}
putchar('\n');
exit(0);

}

