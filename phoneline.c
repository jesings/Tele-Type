#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>

int main(){
    key_t mykey = ftok("tele.c",'R');
    int shmid = shmget(mykey,2048,0), semid = semget(mykey,1,0);
    struct sembuf sb;
    sb.sem_num = 0,sb.sem_flg=0,sb.sem_op = -1;
    semop(semid,&sb,1);    
    if((shmid==-1)||(semid==-1)){
        printf("Shmget/Semget failure\n");
        return -1;
    }
    char* data = shmat(shmid,NULL,0);
    int pos = strlen(data)-1;
    while(pos>0 && data[pos---1]!='\n') ;
    if(pos>0)
        printf("Last line: %s\n",data+pos+1);
    else if(pos==0)
        printf("Last line: %s\n",data+pos);
    else
        printf("Start of file:\n");
    printf("Please insert the next line of the file: ");
    fflush(stdout);
    char buff[256];
    fgets(buff,256,stdin);
    strcat(data,buff);
    shmdt(data);
    sb.sem_op = 1;
    semop(semid,&sb,1);    
    return 0;
}
