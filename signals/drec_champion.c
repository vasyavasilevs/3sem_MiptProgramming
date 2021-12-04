#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

//we send signals to the processes with kill and describe reactions for these signals by my_handler
//endless console printing expected: ЭРТЭ — ЧЕМПИОН!

pid_t pid = -1;

void my_handler(int signum) {
    //for parent
    if (signum == SIGUSR1) {
        write(1, "ЭРТЭ", strlen("ЭРТЭ"));//not printf because of buffer and race condition
        kill(pid, SIGUSR2);//sending sig2 to child
    }
    //for child
    else {
        write(1, " — ЧЕМПИОН!\n", strlen(" — ЧЕМПИОН!\n"));
        kill(getppid(), SIGUSR1);//sending sig1 to parent
    }
}

int main() {
    (void)signal(SIGUSR2, my_handler);
    (void)signal(SIGUSR1, my_handler);
    pid = fork();
    //child screams champion
    if (pid == 0) {
        kill(getppid(), SIGUSR1);//for the very first sending
        while(1);
    }
    //parent screams RT
    else if (pid > 0) {
        while(1);
    }
    return 0;
}