#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
   	pid_t pid = fork();
	if (pid == 0) {
		execlp("gcc", "gcc", "-Wall", "-o", "hello_VA_for_fork_and_exec", "hello_VA_for_fork_and_exec.c", NULL);
	}
	execlp("./hello_VA_for_fork_and_exec", "./hello_VA_for_fork_and_exec", NULL);
	return 0;
}
