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

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Give a command line argument dummy\n");
		return -1;
	}

	char mode;
	int shmid, semid, fd, size;
	struct stat st;

	if (!strncmp(argv[1], "-c", 2) || !strncmp(argv[1], "-r", 2) || !strncmp(argv[1], "-v", 2)) mode = argv[1][1];
	else {
		printf("Give a valid command line argument dummy\n");
		return -1;
	}

	key_t mykey = ftok("tele.c", 'R');
	if (mykey) {
		switch (mode) {
			case 'c':
				shmid = shmget(mykey, 2048, 0644 | IPC_CREAT);
				semid = semget(mykey, 1, 0644 | IPC_CREAT | IPC_EXCL);
				if((shmid == -1) || (semid == -1)) {
					printf("Error: %s\n", strerror(errno));
					return -1;
				}
				semctl(semid, 0, SETVAL, 1);

				fd = open("story", O_CREAT | O_TRUNC, 0644);
				close(fd);
				break;
			case 'r':
				shmid = shmget(mykey, 2048, 0);
				semid = semget(mykey, 1, 0);
				if ((shmid == -1) || (semid == -1)) {
					printf("Error: %s\n", strerror(errno));
					return -1;
				}

				int e = shmctl(shmid, IPC_RMID, NULL);
				if (e == -1) printf("Error: %s\n", strerror(errno));
				e = semctl(semid, IPC_RMID, 0);
				if (e == -1) printf("Error: %s\n", strerror(errno));

				stat("story", &st);
				size = (int)st.st_size;

				fd = open("story", O_RDONLY);
				char *buff = malloc(size);
				read(fd, buff, size);
				close(fd);
				remove("story");

				printf("Your story:\n%s\n", buff);
				free(buff);

				break;
			case 'v':
				stat("story", &st);
				size = (int)st.st_size;

				fd = open("story", O_RDONLY);
				char *buff2 = malloc(size);
				read(fd, buff2, size);
				close(fd);

				printf("This is the story thus far:\n%s\n", buff2);
				free(buff2);
				break;
		}
	} else {
		printf("Idiotic key failure\n");
		return -1;
	}

	return 0;
}
