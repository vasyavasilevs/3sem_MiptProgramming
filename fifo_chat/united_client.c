#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_STRING_SIZE 100

int main(int argc, char *argv[]) {
   int fd = 0;
   pid_t pid = 0;
   char input_string[MAX_STRING_SIZE] = {0}, output_string[MAX_STRING_SIZE] = {0};
   char name_1[] = "fifo12.fifo";
   char name_2[] = "fifo21.fifo";
   
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
	        printf("Input message:");
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
            read(fd, output_string, MAX_STRING_SIZE);
            printf("Output message: %s\n", output_string);
        }
    close(fd);
    }
    return 0;
}
