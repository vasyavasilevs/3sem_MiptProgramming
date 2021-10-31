/* Программа 2 для иллюстрации работы с разделяемой памятью*/

//печатает

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {

    char *array; /* Указатель на разделяемую память */

    int shmid; /* IPC дескриптор для области разделяемой памяти */

    int i = 0;
    
    int new = 1; /* Флаг необходимости инициализации элементов массива */

    char pathname[] = "06-1a.c"; /* Имя файла, использующееся для генерации ключа. Файл с таким именем должен существовать в текущей директории */

    key_t key; /* IPC ключ */

    /* Генерируем IPC ключ из имени файла 06-1a.c в текущей директории и номера экземпляра области разделяемой памяти 0 */

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    /* Пытаемся эксклюзивно создать разделяемую память для сгенерированного ключа, т.е. если для этого ключа она уже существует системный вызов вернет отрицательное значение. Права доступа 0666 - чтение и запись разрешены для всех */

    if ((shmid = shmget(key, 50 * sizeof(char), 0666 | IPC_CREAT | IPC_EXCL)) < 0) {

        /* В случае возникновения ошибки пытаемся определить: возникла ли она из-за того, что сегмент разделяемой памяти уже существует или по другой причине */

        if (errno != EEXIST) {

            /* Если по другой причине - прекращаем работу */

            printf("Can\'t create shared memory\n");
            exit(-1);
        }
        else {

            /* Если из-за того, что разделяемая память уже существует пытаемся получить ее IPC дескриптор и, в случае удачи, сбрасываем флаг необходимости инициализации элементов массива */

            if ((shmid = shmget(key, 50 * sizeof(char), 0)) < 0) {
                printf("Can\'t find shared memory\n");
                exit(-1);
            }
            new = 0;
        }
    }

    if ((array = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)) {
        printf("Can't attach shared memory\n");
        exit(-1);
    }

    printf("%s", array);

    if (shmdt(array) < 0) {
        printf("Can't detach shared memory\n");
        exit(-1);
    }

    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
