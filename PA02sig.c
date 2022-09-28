#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>

pid_t pid;
bool pressed = true;

// Z Handler
void zHandler(int signal) {
	// If press Z for the first time.
    if (pressed) {
		// False condition.
        pressed = false;
        printf("\nChild is PAUSED\n");
		// Pauses the child by sending a stop signal.
        kill(pid, SIGSTOP);
    }
	// When Z is pressed again.
    else {
		// Set pressed to true again.
        pressed = true;
        printf("\nChild is RESUMED\n");
		// Resume the child's task. by sending a continue signal.
        kill(pid, SIGCONT);
    }
}

// C Handler
void cHandler(int signal) {
	// Kills the child process by sending a kill signal.
    kill(pid,SIGKILL);
    printf("\nChild is KILLED. Exiting\n");
    exit(1);
}

int main() {
    char *args[] = {"bin/yes","y",0};
    char *env[] = {0};

    pid = fork();

    if (pid == 0) {
		// Child run "yes" command.
        execve("/bin/yes",args,env);
    }
    else {
		// Struct handler for Z.
        struct sigaction ctrlZ;
        ctrlZ.sa_handler = zHandler;
        sigemptyset(&ctrlZ.sa_mask);
        ctrlZ.sa_flags = 0;

		// SIGSTP for Z (LINUX)/
        sigaction(SIGTSTP,&ctrlZ,0);

		// Struct handler for C.
        struct sigaction ctrlC;
        ctrlC.sa_handler = cHandler;
        sigemptyset(&ctrlC.sa_mask);
        ctrlC.sa_flags = 0;

		// SIGINT for C (LINUX).
        sigaction(SIGINT,&ctrlC,0);

        while (1) {}
    }

    return 0;
}
