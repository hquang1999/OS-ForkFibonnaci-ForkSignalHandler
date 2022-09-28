#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

void fib_forking(int x) {
	pid_t pid;
	long msg;
	long out;
	int fd;

	// opens the pipe to read and write
	if ((fd = open("myfifo", O_RDWR)) < 0) {
		perror("open failure");
		exit(2);
	}
	
	if (x == 1) {
		msg = 1;
		// (void*) &type is a cast for primitives to const void *buf
		// writes 1 to pipe if we recurse down to fib(1)
		write(fd, (void*) &msg, sizeof(long));
		return;
	}
	else if (x == 0) {
		msg = 0;
		// writes 0 to pipe if we recurse down to fib(0)
		write(fd, (void*) &msg, sizeof(long));
		return;
	}
	else {	
		switch (pid = fork()) {
			case -1:
				{
					perror("fork failure");
					exit(2);
					break;
				}
			case 0:
				{
					// children node
					fib_forking(x - 1);
					exit(0);
					break;
				}
			default:
				{
					// parent node
					wait(NULL);
					fib_forking(x - 2);

					read(fd, (void*) &out, sizeof(long));
					long m1 = out;		

					read(fd, (void*) &out, sizeof(long));
					long m2 = out;		

					long res = m1 + m2;
					write(fd, (void*) &res, sizeof(long));
					
					break;
				}
		}
	}
	return;
}

int fib_seq(int x) { /* slow/recursive implementation of Fib */
	int i, rint = (rand()%30); double dummy;
	
	for (i=0; i<rint*100; i++) {dummy=2.345*i*8.765/1.234;}
	
	if (x==0) return(0); 
		else if (x==1) return(1); 
		else return(fib_seq(x-1)+fib_seq(x-2));
}

int main (int argc, char* argv[]) {
	// general variables
	int n = 0;
	int m = 0;
	long out;
	
	// variables for the pipe
	int opt = 0;
	extern int errno;
	int fd;

	// make named pipe called myfifo
	if (mkfifo("myfifo",0666) < 0 && errno != EEXIST) {
		perror("mkfifo failure");
		exit(1);
	}

	// getopt checking for out inputs
	while ((opt = getopt(argc,argv,"F:S:")) != -1) {
		switch (opt) {
			case 'F':
				n = atoi(optarg);
				break;
			case 'S':
				m = atoi(optarg);
				break;
			default:
				printf("Invalid Inputs\n");
				break;
		}
	}

	// if any input is < 0, close
	if ((n < 0) || (m < 0)) {
		printf("Invalid Inputs\n");
	}
	else {
		if ((n - 2) <= m) {
			int seq = fib_seq(m);
			printf("fib(%d) = %d\n", m, seq);
		}
		else {
			fib_forking(n);

			// opens the pipe to read and write
			if ((fd = open("myfifo", O_RDWR)) < 0) {
				perror("open failure");
				exit(2);
			}
			
			// reads final result from pipe
			read(fd,(void*)&out, sizeof(long));
			printf("fib(%d) = %ld\n", n, out);
		}
	}
	return 0;
}
