#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_STRING_SIZE 100

/*
/Поправила первый проблемный запуск с mknod и убрала бесконечную печать 
/output одного из клиентов при завершении второго.
*/

int main(int argc, char *argv[]) {
    int fd = 0;
    int i = 8;
    int j = 8;
    size_t size;
    pid_t pid = 0;
    char input_string[MAX_STRING_SIZE] = {0}, output_string[MAX_STRING_SIZE] = {0};
    char name_1[] = "fifo12.fifo";
    char name_2[] = "fifo21.fifo";
   
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
    //множим процессы
    if ((pid = fork()) < 0) {
      printf("Can\t fork child\n");
      exit(-1);
    } 
    //родитель
    else if (pid > 0) {
        //открываем для записывания
        if (atoi(argv[1]) == 1) {
           fd = open(name_1, O_WRONLY); //для первого клиента
       }
        else {
           fd = open(name_2, O_WRONLY); //для второго клиента
       }
        //записываем в фифо из консоли
        while (1) {
	    printf("Input message: ");
            fgets(input_string, MAX_STRING_SIZE, stdin);
            write(fd, input_string, MAX_STRING_SIZE);
      }
      close(fd);
    } 
    //ребёнок
    else {
        //открываем для чтения
        if (atoi(argv[1]) == 1) {
            fd = open(name_2, O_RDONLY);//первый клиент
        }
        else {
            fd = open(name_1, O_RDONLY);//второй клиент
        }
        //читаем и печатаем
        while (1) {
            size = read(fd, output_string, MAX_STRING_SIZE);
            printf("\nOutput message: %s", output_string);
            if(size == 0) {
            printf("Can\'t read string from FIFO21\n");
            exit(-1);
            }
        }
    close(fd);
    }
    return 0;
}
