#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/*
При запуске передать аргументом командной строки имя искомого файла (с расширением)
*/

/*
The type DIR, which is defined in the header <dirent.h>, represents a directory stream, which is an 
ordered sequence of all the directory entries in a particular directory. Directory entries represent files. 
Files can be removed from a directory or added to a directory asynchronously to the operation of readdir() and readdir_r().  

readdir()
The readdir() function returns a pointer to a structure representing the directory entry at the current position in 
the directory stream specified by the argument dirp, and positions the directory stream at the next entry. It returns a null 
pointer upon reaching the end of the directory stream. The structure dirent defined by the <dirent.h> header describes a directory entry.


struct dirent {
               ino_t          d_ino;        Inode number 
               off_t          d_off;        Not an offset; see below 
               unsigned short d_reclen;     Length of this record 
               unsigned char  d_type;       Type of file; not supported by all filesystem types 
               char           d_name[256];  Null-terminated filename 
           };
*/

/*
struct stat {
    dev_t         st_dev;       устройство 
    ino_t         st_ino;       inode 
    mode_t        st_mode;      режим доступа 
    nlink_t       st_nlink;     количество жестких ссылок 
    uid_t         st_uid;       идентификатор пользователя-владельца 
    gid_t         st_gid;       идентификатор группы-владельца 
    dev_t         st_rdev;      тип устройства 
                                (если это устройство) 
    off_t         st_size;      общий размер в байтах 
    blksize_t     st_blksize;   размер блока ввода-вывода 
                                в файловой системе 
    blkcnt_t      st_blocks;    количество выделенных блоков 
    time_t        st_atime;     время последнего доступа 
    time_t        st_mtime;     время последней модификации 
    time_t        st_ctime;     время последнего изменения 
};

Указанные далее макросы POSIX проверяют, является ли файл:

S_ISLNK(m)
символьной ссылкой (Нет в POSIX.1-1996.)
S_ISREG(m)
обычным файлом
S_ISDIR(m)
каталогом
S_ISCHR(m)
символьным устройством
S_ISBLK(m)
блочным устройством
S_ISFIFO(m)
каналом FIFO
S_ISSOCK(m)
сокетом
*/


void FindFile (char *cur_dir, char *file_name, int depth, int *number_of_found_files) {
    struct stat tmp;
    struct dirent *directory;
    DIR *direct = opendir(cur_dir);
    if (direct == NULL) {
		perror("Ошибка при открытии cur_dir");
		return;
	}
    if (depth == 0)
        return;
    char *route = (char *)calloc(1, sizeof(char));//выделение памяти под маршрут
    while ((directory = readdir(direct)) != NULL) { //пока объекты в DIR читаются (то есть пока не прочитаны все объекты)
        route = realloc(route, sizeof(char) * (strlen(cur_dir) + strlen(directory->d_name) + 8)); //расширение памяти под маршрут
        strcpy(route, cur_dir); //копируем название текущей директории в маршрут
        strcat(route, "/");//добавляем в маршрут слеш
        strcat(route, directory->d_name);//добавляем в маршрут текущий читаемый объект
        stat(route, &tmp);//для получения информации об объекте
        if (S_ISDIR(tmp.st_mode) && depth > 0) {            //если директория, рекурсивно продолжаем поиск
            if ((strcmp(directory->d_name, "..") != 0) && (strcmp(directory->d_name, ".") != 0)) { //ls -a, всегда есть скрытые директории такого типа, их проверку исключаем
                FindFile(route, file_name, depth - 1, number_of_found_files);
            } 
        }
        else if (S_ISREG(tmp.st_mode) && strcmp(file_name, directory->d_name) == 0) { //если файл, и имя этого файла совпадает с искомым, печатаем, что нашли этот файл
            printf("Файл найден, путь к файлу: %s\n", route);
            ++(*number_of_found_files);
            continue; 
        }
    }
    free(route);
    closedir(direct);
    return;
}

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("Неверный формат ввода аргументов командной строки\n");
        return 0;
    }
    int deep = 0;
    int counter = 0;
    printf("Введите глубину поиска (количество проверяемых директорий, начиная с текущей): ");
    scanf("%d", &deep);
    char current_direct[8] = ".";
    FindFile(current_direct, argv[1], deep, &counter);
    if (counter != 0) {
        printf("Найдено %d шт файлов с названием %s\n", counter, argv[1]);
    }
    else 
        printf("Файл не найден\n");
    return 0;
}
