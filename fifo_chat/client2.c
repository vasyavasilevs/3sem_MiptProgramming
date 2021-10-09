#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_STRING_SIZE 100

int main() {
   int fd = 0;
   pid_t pid = 0;
   size_t size;
   char input_string[MAX_STRING_SIZE] = {0};
   char output_string[MAX_STRING_SIZE] = {0};
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

   if((pid = fork()) < 0) {
      printf("Can\t fork child\n");
      exit(-1);

   } else if (pid > 0) {

      //записываем в фифо21
      if((fd = open(name_2, O_WRONLY)) < 0){
         printf("Can\'t open FIFO21 for writting\n");
         exit(-1);
      }

      while (1) {
      fgets(input_string, MAX_STRING_SIZE, stdin);

      size = write(fd, input_string, MAX_STRING_SIZE);

      if(size > MAX_STRING_SIZE){
        printf("Can\'t write all string to FIFO21\n");
        exit(-1);
      }
      }

      close(fd);
      printf("Parent exit\n");

} else {

      //открываем для рида

      if ((fd = open(name_1, O_RDONLY)) < 0){
         printf("Can\'t open FIFO12 for reading\n");
         exit(-1);
      }
      while(1) {
      size = read(fd, output_string, MAX_STRING_SIZE);

      if(size < 0){
         printf("Can\'t read string from FIFO12\n");
         exit(-1);
      }

      //печатаем

      printf("Child exit, output_string: %s\n", output_string);
      }
      close(fd);
   }
   return 0;
}


