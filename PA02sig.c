#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>

pid_t pid;
bool pressed = true;

void zHandler(int signal) {
    if (pressed) {
        pressed = false;
        printf("\nChild is PAUSED\n");
        kill(pid, SIGSTOP);
    }
    else {
        pressed = true;
        printf("\nChild is RESUMED\n");
        kill(pid, SIGCONT);
    }
}

void cHandler(int signal) {
    kill(pid,SIGKILL);
    printf("\nChild is KILLED. Exiting\n");
    exit(1);
}

int main() {
    char *args[] = {"bin/yes","y",0};
    char *env[] = {0};

    pid = fork();

    if (pid == 0) {
        execve("/bin/yes",args,env);
    }
    else {
        struct sigaction ctrlZ;
        ctrlZ.sa_handler = zHandler;
        sigemptyset(&ctrlZ.sa_mask);
        ctrlZ.sa_flags = 0;

        sigaction(SIGTSTP,&ctrlZ,0);

        struct sigaction ctrlC;
        ctrlC.sa_handler = cHandler;
        sigemptyset(&ctrlC.sa_mask);
        ctrlC.sa_flags = 0;

        sigaction(SIGINT,&ctrlC,0);

        while (1) {}
    }

    return 0;
}
