#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/* Создать N процессов. Каждый i-й процесс
 * создаёт один дочерний i + 1 и дожидается его завершения.
 * На экран выводится информация о
 * создании нового процесса + от каждого
 * родительского, что его ребёнок завершился.
 */

int main() {
	int i = 0, N = 0;
	printf("\nВведите количество дочерних процессов (длину дерева): ");
	scanf("%d", &N);
	printf("\n");
	int status[N];
	for (i = 0; i < N; i++)
	{
		pid_t pid = fork();
		if (pid > 0) {
		    printf("Я %d родитель, буду ждать завершения ребенка, который печатает %d\n", getpid(), i);
		    int id = wait(&(status[i]));
		    printf("Я %d родитель, мой ребенок %d завершился\n\n", getpid(), id);
		    exit(0);
		}
		else if (pid == 0) {
                    printf("Я %d родился, мой родитель -  %d, печатаю %d\n\n", getpid(), getppid(), i);
                }
	}
	return 0;
}

