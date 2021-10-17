#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define MAX_STRING_SIZE 100

/*
/Оказалось, что мы после семинара не совсем до конца исправили, работало
/только в одном направлении (первый клиент печатал, другой висел на риде).
/Это было из-за глобальной переменной fd. Заменила ее на локальные в функциях,
/поправила первый проблемный запуск с mknod и убрала бесконечную печать 
/output одного из клиентов при завершении второго.
*/

int result1 = 0, result2 = 0;
char input_string[MAX_STRING_SIZE] = {0}, output_string[MAX_STRING_SIZE] = {0};
char name_1[] = "fifo12.fifo";
char name_2[] = "fifo21.fifo";

void *mythread1(void *argv1) {
	pthread_t mythid;//для идентификатора нити исполнения
	mythid = pthread_self();
	int fd = 0;
	int x = *((int *) argv1);
	if (x == 1) {
           fd = open(name_1, O_WRONLY);//для первого клиента
        }
        else {
           fd = open(name_2, O_WRONLY);//для второго клиента
        }
        //записываем в фифо из консоли
        while (1) {
	    printf("Input message: ");
            fgets(input_string, MAX_STRING_SIZE, stdin);
            write(fd, input_string, MAX_STRING_SIZE);
      	}
      	close(fd);
	return NULL;
}

void *mythread2(void *argv1) {
	pthread_t mythid;//для идентификатора нити исполнения
	mythid = pthread_self();
	int fd = 0;
	size_t size;
	int x = *((int *) argv1);
	if (x == 1) {
            fd = open(name_2, O_RDONLY);//первый клиент
        }
        else {
            fd = open(name_1, O_RDONLY);//второй клиент
        }
        //читаем и печатаем
        while (1) {
            size = read(fd, output_string, MAX_STRING_SIZE);
            printf("\nOutput message: %s", output_string);
            if (size == 0) {
                printf("Can\'t read string from FIFO21\n");
                exit(-1);
            }
        }
    	close(fd);
	return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thid1, thid2;
    int i = 8, j = 8;
    int argv1 = atoi(argv[1]);
   
    i = mknod(name_1, S_IFIFO | 0666, 0);
    if (i != 0) {
       if (errno != EEXIST) {
           printf("Can\'t create FIFO12\n");
           exit(-1);
       }
    }
   	
    j = mknod(name_2, S_IFIFO | 0666, 0);
    if (j != 0) {
       if (errno != EEXIST) {
           printf("Can\'t create FIFO21\n");
           exit(-1);
       }
    }
    //first thread
    result1 = pthread_create(&thid1, (pthread_attr_t *)NULL, mythread1, &argv1);
    if(result1 != 0) {
	printf ("Error on thread create, return value = %d\n", result1);
	exit(-1);
    }
    //second thread
    result2 = pthread_create(&thid2, (pthread_attr_t *)NULL, mythread2, &argv1);
    if(result2 != 0) {
	printf ("Error on thread create, return value = %d\n", result2);
	exit(-1);
    }
    pthread_join(thid1, NULL);
    pthread_join(thid2, NULL);
    return 0;
}
