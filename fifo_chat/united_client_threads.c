#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define MAX_STRING_SIZE 100

int fd = 0;
int result1 = 0, result2 = 0;
char input_string[MAX_STRING_SIZE] = {0}, output_string[MAX_STRING_SIZE] = {0};
char name_1[] = "fifo12.fifo";
char name_2[] = "fifo21.fifo";

void *mythread1(void *argv1) {
	pthread_t mythid; /* Для идентификатора нити исполнения */
	mythid = pthread_self();
	if ((int) argv1 == 1) {
           fd = open(name_1, O_WRONLY); //для первого клиента
        }
        else {
           fd = open(name_2, O_WRONLY); //для второго клиента
        }
        //записываем в фифо из консоли
        while (1) {
	    printf("Input message:");
            fgets(input_string, MAX_STRING_SIZE, stdin);
            write(fd, input_string, MAX_STRING_SIZE);
      	}
      	close(fd);
	printf("Thread %d \n", mythid);
	return NULL;
}

void *mythread2(void *argv1) {
	pthread_t mythid; /*Для идентификатора нити исполнения */
	mythid = pthread_self();
	if ((int) argv1 == 1) {
            fd = open(name_2, O_RDONLY);//первый клиент
        }
        else {
            fd = open(name_1, O_RDONLY);//второй клиент
        }
        //читаем и печатаем
        while (1) {
            read(fd, output_string, MAX_STRING_SIZE);
            printf("Output message: %s\n", output_string);
        }
    	close(fd);
	printf("Thread %d \n", mythid);
	return NULL;
}

int main(int argc, char *argv[]) {
   pthread_t thid, mythid;
   
   int argv1 = atoi(argv[1]);
   
   mknod(name_1, S_IFIFO | 0666, 0);
   if (errno != EEXIST) {
      printf("Can\'t create FIFO12\n");
      exit(-1);
   }
   mknod(name_2, S_IFIFO | 0666, 0);
   if (errno != EEXIST) {
      printf("Can\'t create FIFO21\n");
      exit(-1);
   }
   //first thread
   result1 = pthread_create(&thid, (pthread_attr_t *)NULL, mythread1(argv1), NULL);
    if(result1 != 0) {
	printf ("Error on thread create, return value = %d\n", result1);
	exit(-1);
    }
    //second thread
    result2 = pthread_create(&thid, (pthread_attr_t *)NULL, mythread2(argv1), NULL);
    if(result2 != 0) {
	printf ("Error on thread create, return value = %d\n", result2);
	exit(-1);
    }
    return 0;
}
