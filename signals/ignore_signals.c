#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

//we make SIGQUIT ignored and make SIGINT printing its number

void my_handler(int nsig) {
    printf("\n%d\n", nsig);
}

int main() {
    (void)signal(SIGQUIT, SIG_IGN);
    (void)signal(SIGINT, my_handler);
    while(1);
    return 0;
}