#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

*/Я перестала понимать, что здесь происходит. Отладочная информация вводит в ещё более крупные вопросы.
Если, например, сделать в одном из процессов -2 в качестве операции захвата жетона, то работает сначала только один процесс (как 
и должно быть по идее). А дальше второй пишет ерроры и все равно печатает. Хотела понять этап "взял"-"положил", но 
не осознаю, с какого момента ломается. Большая загадка в том, почему два раза выводит ключ и семид (как будто один из процессов выполняет по второму кругу
то, что уже было выполнено до форка. Help, please*/

//struct sembuf mybuf;
//int semid;

void Semaf (int semid, int n) {
	struct sembuf mybuf;	
	if (n == -1)
		printf("взял\n");
	else
		printf("положил\n");
	mybuf.sem_op = n;
	mybuf.sem_flg = 0;
	mybuf.sem_num = 0;
	if (semop(semid, &mybuf, 1) < 0) 
		printf("error");
}

int main () {
	int i = 0;
	int semid; 
	char pathname[] = "hello.txt"; 

	key_t key; 

	if((key = ftok(pathname,0)) < 0){
		printf("Can\'t generate key\n");
		exit(-1);
	}
	
	//printf("kek");
	printf("%d\n", key);
	
	if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
		printf("Can\'t get semid\n");
		exit(-1);
	}

	printf("%d\n", semid);
	
	Semaf(semid, 1);
	
	pid_t pid = fork();
	
	if (pid == 0) {
		if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
			printf("Can\'t get semid\n");
			exit(-1);
		}
		for (i = 0; i < 500; i++) {
			Semaf(semid, -1);
			printf("Прекрасный - льщу себя надеждой в делах, где я кругом невежда.");
			printf("\n");
			Semaf(semid, 1);
		}
		exit(0);
	}
	
	
	
	else if (pid > 0) {
		if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
			printf("Can\'t get semid\n");
			exit(-1);
		}
		for (i = 0; i < 500; i++) {
			Semaf(semid, -1);
			printf("Я лежу от вас на пядь на навозной куче. Не поможете ли встать Вы звезде падучей?");
			printf("\n");
			Semaf(semid, 1);
		}
		//exit(0);
	}
	/*pid = fork();
	if (pid == 0) {
		if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
			printf("Can\'t get semid\n");
			exit(-1);
		}
		while(i != 0) {
			Semaf(semid, -1);
			printf("Ты говоришь, как сластолюб-француз, прошу меня не торопить, однако. Не понимаю, право, что за вкус в глотанье наспех лакомства, без смаку? Приятно то, что отдаляет цель: улыбки, вздохи, встречи у фонтана, печаль томленья, - словом, канитель, которою всегда полны романы.\n");
			Semaf(semid, 1);
			i -= 1;
		}
		exit(0);
	}
	pid = fork();
	if (pid == 0) {
		if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
			printf("Can\'t get semid\n");
			exit(-1);
		}
		while(i != 0) {
			Semaf(semid, -1);
			printf("Я лежу от вас на пядь на навозной куче. Не поможете ли встать Вы звезде падучей?\n");
			Semaf(semid, 1);
			i -= 1;
		}
		exit(0);
	}*/
	semctl(semid, 0, IPC_RMID, 0);
    	return 0;
}
