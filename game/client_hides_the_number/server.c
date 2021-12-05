#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

//run with gcc -Wall -o server server.c -lpthread
//программа на основе сервера TCP из учебника

int flag = 0;//флажок для обработки загаданного числа
char hidden_number[999];//массив для хранения загаданного числа, обрабатывается в нити

typedef struct {
	int sockfd; //Дескрипторы для слушающего и присоединенного сокетов 
	int newsockfd;
	int n; //Количество принятых символов
    char line[1000]; //Буфер для приема информации
} Help;

void* Thread(void* help) {
    Help helpp = *((Help *)help);
    /* В цикле принимаем информацию от клиента до
        тех пор, пока не произойдет ошибки (вызов read()
        вернет отрицательное значение) или клиент не
        закроет соединение (вызов read() вернет 
        значение 0). Максимальную длину одной порции 
        данных от клиента ограничим 999 символами. В
        операциях чтения и записи пользуемся дескриптором
        присоединенного сокета, т. е. значением, которое
        вернул вызов accept().*/
    while((helpp.n = read(helpp.newsockfd, helpp.line, 999)) > 0) {
            if (flag == 0) {
                strcpy(hidden_number, helpp.line);//сохраняем первое отправленное любым из клиентов число, оно загадано и теперь все его будут отгадывать
            }
            if ((strcmp(helpp.line, hidden_number) == 0) && flag == 0) {
                if((helpp.n = write(helpp.newsockfd, "Hidden number saved!\n", strlen("Hidden number saved!\n")+1)) < 0) {
                    perror(NULL);
                    close(helpp.sockfd);
                    close(helpp.newsockfd);
                    exit(1);
                }
                flag = 1;
                continue;//чтобы сигкилл не вызывался сразу после сохранения числа
            }
            if ((strcmp(helpp.line, hidden_number) == 0) && flag == 1) {
                if((helpp.n = write(helpp.newsockfd, "It is a success!\n", strlen("It is a success!\n")+1)) < 0) {
                    perror(NULL);
                    close(helpp.sockfd);
                    close(helpp.newsockfd);
                    exit(1);
                }
                kill(getpid(), SIGKILL);//решила отключать сервер после того, как кто-нибудь из клиентов отгадает число, чтобы он больше не отвечал другим
            }
            if((helpp.n = write(helpp.newsockfd, "Please, try again\n", strlen("Please, try again\n")+1)) < 0) {
                perror(NULL);
                close(helpp.sockfd);
                close(helpp.newsockfd);
                exit(1);
            }
        }
    /* Если при чтении возникла ошибка – завершаем работу */
    if(helpp.n < 0) {
        perror(NULL);
        close(helpp.sockfd);
        close(helpp.newsockfd);
        exit(1);
    }
    /* Закрываем дескриптор присоединенного сокета и
    уходим ожидать нового соединения */
    close(helpp.newsockfd);
}


int main() {
    Help help;
    struct sockaddr_in servaddr, cliaddr; /* Структуры 
        для размещения полных адресов сервера и 
        клиента */
    int clilen; /* Длина адреса клиента */
    /* Создаем TCP-сокет */
    if((help.sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(NULL);
        exit(1);
    }
    /* Заполняем структуру для адреса сервера: семейство
    протоколов TCP/IP, сетевой интерфейс – любой, номер 
    порта 51000. Поскольку в структуре содержится 
    дополнительное не нужное нам поле, которое должно 
    быть нулевым, обнуляем ее всю перед заполнением */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family= AF_INET;
    servaddr.sin_port= htons(51000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Настраиваем адрес сокета */
    if(bind(help.sockfd, (struct sockaddr *) &servaddr, 
    sizeof(servaddr)) < 0){
        perror(NULL);
        close(help.sockfd);
        exit(1);
    }
    /* Переводим созданный сокет в пассивное (слушающее) 
    состояние. Глубину очереди для установленных 
    соединений описываем значением 5 */
    if(listen(help.sockfd, 5) < 0) {
        perror(NULL);
        close(help.sockfd);
        exit(1);
    }
    /* Основной цикл сервера */
    while(8) {
        /* В переменную clilen заносим максимальную
        длину ожидаемого адреса клиента */
        clilen = sizeof(cliaddr);
        /* Ожидаем полностью установленного соединения
        на слушающем сокете. При нормальном завершении 
        у нас в структуре cliaddr будет лежать полный 
        адрес клиента, установившего соединение, а в 
        переменной clilen – его фактическая длина. Вызов
        же вернет дескриптор присоединенного сокета, через
        который будет происходить общение с клиентом. 
        Заметим, что информация о клиенте у нас в
        дальнейшем никак не используется, поэтому 
        вместо второго и третьего параметров можно 
        было поставить значения NULL. */
        if((help.newsockfd = accept(help.sockfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
            perror(NULL);
            close(help.sockfd);
            exit(1);
        }
        pthread_t thid1;
        int result = 0;
        result = pthread_create(&thid1, (pthread_attr_t *)NULL, Thread, &help);
        if(result != 0) {
	    printf ("Error on thread create, return value = %d\n", result);
	    exit(-1);
        }
    }
    return 0;
}
