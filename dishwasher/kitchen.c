#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

/*
Запуск:
0 - washer
1 - dryer
*/

#define TABLE_LIMIT 4
#define TMP_SIZE 10

int semid;

//ф-ция для действий над семафорами
void Semaf (int n) {
	struct sembuf mybuf;
	mybuf.sem_op = n;
	mybuf.sem_flg = 0;
    	mybuf.sem_num = 0;
	if (semop(semid , &mybuf , 1) < 0) {
		printf("Error\n");
		exit(-1);
	}
}

int main(int argc, char *argv[]) {
	int common_fifo = 0;
    	char* skill[] = {"washer.txt", "dryer.txt"};

	int dish_type = -1;
	char tmp[TMP_SIZE];

	int skill_id = atoi(argv[1]);

	//создаем семафор
	char pathname[] = "mem.txt";

	key_t key;

	if((key = ftok(pathname,0)) < 0) {
		printf("Can\'t generate key\n");
		exit(-1);
	}

	if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
		printf("Can\'t get semid\n");
		exit(-1);
	}

	//фифо для стола
	char table_fifo[] = "table.fifo";

	if ((mknod(table_fifo , S_IFIFO | 0666 , 0) < 0) && (errno != EEXIST)) {
		printf("Can`t create FIFO\n");
		exit(-1);
	}

	if (atoi(argv[1]) == 0) {
		common_fifo = open(table_fifo, O_WRONLY);//для того, кто моет
	}
	else {
		common_fifo = open(table_fifo, O_RDONLY);//для того, кто вытирает
	}

	FILE* fp = NULL;
	fp = fopen(skill[skill_id], "r");//washer|dryer на чтение в зависимости от агрумента КС

	int max_dish = 0;
	int current_dish, time = 0;

	//ищем самую большую тарелку, чтобы потом создать массив подходящего размера
	while (fscanf(fp, "%d:%d\n", &current_dish, &time) == 2) {
		if (current_dish > max_dish)
			max_dish = current_dish;
	}

	rewind(fp);//в начало файла

	int *skill_time = (int *)calloc(sizeof(int), max_dish + 1);

	while (fscanf(fp, "%d:%d\n", &current_dish, &time) == 2) {
		skill_time[current_dish] = time;
	}
	fclose(fp);

	//washer
	if (skill_id == 0) {

		int numb_of_dishes = 0;
		int i = 0;

		FILE* dirty_dishes = NULL;
		dirty_dishes = fopen("dirty_dishes.txt", "r");//открываем на чтение грязную посуду

		Semaf(TABLE_LIMIT);//заполняем семафор жетонами в количестве максимальной вместимости стола

		while (1) {
			fscanf(dirty_dishes, "%d:%d", &dish_type, &numb_of_dishes);//считываем тип посуды и количество тарелок
			if (dish_type == 0) {
				sprintf(tmp, "%d\n", dish_type);//записываем в tmp тип посуды
				write(common_fifo, tmp, TMP_SIZE);//записываем tmp в общий фифо, чтобы dryer узнал тип посуды aka кладем на стол
				printf("Человек, который моет посуду, завершил работу\n");
				break;
			}
			//если диштайп не ноль, то цикл по количеству посуды данного типа
			for (i = 0; i < numb_of_dishes; i++) {
				sleep(skill_time[dish_type]);//спим столько, сколько надо для мытья тарелки aka моем тарелку столько времени
				sprintf(tmp, "%d\n", dish_type);
				write(common_fifo, tmp, TMP_SIZE);//кладем на стол (эта штука работает по причине существования буфера fifo, в который записываются ->
				//-> новые данные, читаемые reader-ом с задержкой, в конечном итоге reader ничего не потеряет и прочитает все, что было записано, но возможно с опозданием)
				printf("Человек, который моет посуду, помыл тарелку типа %d и положил ее на стол\n", dish_type);
				Semaf(-1);//и так до конца места на столе, потом ждет, пока тот, кто вытирает, уберет со стола хотя бы одну тарелку
				printf("Сейчас на столе %d тарелок/тарелки\n", TABLE_LIMIT - semctl(semid, 0, GETVAL));
			}		
		}
		fclose(dirty_dishes);
	}
	//dryer
	else {
		while (1) {
			read(common_fifo, tmp, TMP_SIZE);
			dish_type = atoi(tmp);
			//индикатор конца работы
			if (dish_type == 0) {
				printf("Человек, который вытирает посуду, завершил работу\n");
				break;
			}
			sleep(skill_time[dish_type]);//протираем посуду столько времени
			printf("Человек, который вытирает посуду, протер тарелку типа %d и убрал ее со стола\n", dish_type);
			Semaf(1);//освобождает место для новой тарелки на столе
			printf("Сейчас на столе %d тарелок/тарелки\n", TABLE_LIMIT - semctl(semid, 0, GETVAL));
		}
	}
	close(common_fifo);

	if (skill_id == 1) {
		semctl(semid, 0, IPC_RMID, 0); //удалили созданный семафор
	}

	return 0;
}
