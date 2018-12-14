#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};

int main() {
	struct sembuf sb;

	key_t mykey = ftok("tele.c", 'R');
	int shmid = shmget(mykey, 2048, 0);
	int semid = semget(mykey, 1, 0);

	sb.sem_num = 0, sb.sem_flg = SEM_UNDO, sb.sem_op = -1;
	if ((shmid == -1) || (semid == -1)) {
		printf("Error: %s\n", strerror(errno));
		return -1;
	}
	printf("Checking the availability of the shared memory segment!\n");
	if (!semctl(semid, 0, GETVAL, 1)) {
		printf("Segment unavailable, exiting\n");
		return -1;
	}

	semop(semid, &sb, 1);
	char *data = shmat(shmid, 0, 0);
	if (*data == -1) printf("Error: %s\n", strerror(errno));
	else if (!*data) printf("No story yet!\n");
	else {
		int fd = open("story", O_RDONLY);
		lseek(fd, -1 * *data, SEEK_END); //go back # of bytes in len of data
		char *buff = malloc(*data);
		read(fd, buff, *data);
		printf("Last line: \n%s\n", buff);
		close(fd);
		free(buff);
	}

	printf("Please insert the next line of the file:\n");
	char input[2048];
	fgets(input, sizeof(input), stdin);
	int fd = open("story", O_WRONLY | O_APPEND);
	*data = strlen(input);
	write(fd, input, *data);
	close(fd);
	shmdt(data);
	sb.sem_op = 1;
	semop(semid, &sb, 1);
	return 0;
}
