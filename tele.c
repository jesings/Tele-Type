#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };



int main(int argc,char** argv){
    if(argc<2){
        printf("Give a command line argument dummy\n");
        return -1;
    }
    char mode;
    union semun s;
    if(argv[1][0]=='-'&&(argv[1][1]=='c'||argv[1][1]=='r'||argv[1][1]=='v')){
        mode = argv[1][1];
    }
    else{
        printf("Give a valid command line argument dummy\n");
        return -1;
    }
    key_t mykey = ftok("tele.c",'R');
    int shmid,semid;
    if(mykey){
        switch(mode){
            case 'c':
                shmid = shmget(mykey,2048,0644|IPC_CREAT|IPC_EXCL);
                semid = semget(mykey,1,0644|IPC_CREAT|IPC_EXCL);
                if((shmid==-1)||(semid==-1)){
                    printf("Shm/semcreation failure\n");
                    return -1;
                }
                semctl(semid,0,SETVAL,1);
                break;
            case 'r':
                shmid = shmget(mykey,2048,0);
                semid = semget(mykey,1,0);
                if((shmid==-1)||(semid==-1)){
                    printf("Shmget/Semget failure\n");
                    return -1;
                }
                shmctl(shmid, IPC_RMID, NULL);
                semctl(semid, IPC_RMID, 0);
                break;
            case 'v':
                shmid = shmget(mykey,2048,0);
                if(shmid==-1){
                    printf("Shmget failure\n");
                    return -1;
                }
                char* str = shmat(shmid,NULL,0); 
                printf("This is the story thusfar:\n");
                puts(str);
                break;
        }
    }
    else{
        printf("Idiotic key failure\n");
        return -1;
    }
    return 0;
}
