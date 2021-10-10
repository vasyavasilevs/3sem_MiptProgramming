#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

void Split(char *string, char *delimiters, char ***tokens, int *tokens_count);

// TODO: объявляйте константы в одном стиле: MAX_STRING_SIZE, TIMEOUT ...
#define MAX_STRING_SIZE 1000
#define MAX_TOKEN_SIZE 1000
#define NUMBER_OF_TOKENS 1000
#define TIMEOUT 5

int main () {
    int i = 0, counter_of_strings = 0;
    char stdin_delimiters[] = {' ', '\n', '\0'};
    char stdin_string[MAX_STRING_SIZE] = {0};
    FILE *fp = fopen("commands.txt", "r");

    char **tokens = (char **) malloc(sizeof(char *) * NUMBER_OF_TOKENS);//массив токенов (слов)
    for (i = 0; i < NUMBER_OF_TOKENS; i++) {
        tokens[i] = (char *) calloc (MAX_TOKEN_SIZE, sizeof(char));
    } //выделение памяти под каждый токен (слово)

    fgets(stdin_string, MAX_STRING_SIZE, fp);
    counter_of_strings = atoi(stdin_string); //преобразуем строку в число, с которого эта строка начинается (= 3)
    int child_id[counter_of_strings];
    int sleep_time[counter_of_strings];
 
    for (i = 0; i < counter_of_strings; i++) {
        fgets(stdin_string, MAX_STRING_SIZE, fp);
        int tokens_count = 0;
        Split((char *)stdin_string, (char *)stdin_delimiters, &tokens, &tokens_count);
        sleep_time[i] = atoi(tokens[0]);
        pid_t pid = fork();
        if (pid == 0) {
            tokens[tokens_count] = NULL;//чтобы избежать потери блока динамической памяти это вынесено из Split-a
            sleep(sleep_time[i]); //спит столько, сколько указано в начале каждой строки
            execvp(tokens[1], tokens + 1);
            exit(i);
        }
        else
            child_id[i] = pid;//чтобы родителю было видно id-ы детей для слежки за их временем работы ниже
    }
    // TODO: идея, как отслеживать timeout хорошая. Для теста передайте на вход несколько задача, которые долго работают, и проверьте, все ли они будут убиты?
    // Выводите на экран отладочную информацию. Кажется, что в общем случае убиваете не того, кого требуется.
    for (i = 0; i < counter_of_strings; i++) {
        pid_t killer_pid = fork();
        if (killer_pid == 0) {
            sleep (sleep_time[i]+ TIMEOUT);//спит столько, сколько отслеживаемый процесс, + timeout, в течение которого он работает
            kill (child_id[i], SIGKILL); //принудительно завершить процесс, вышедший за рамки timeout
	        printf("\nProcess %d is killed for being out of time!\n", child_id[i]);
            exit(i);
        }
        else
            child_id[i] = killer_pid;//заполняем старый массив child_id новыми id-ами следящих детей, чтобы потом передать их в waitpid ниже
    }
    for (i = 0; i < NUMBER_OF_TOKENS; i++) {
        waitpid(child_id[i], NULL, 0);//чтобы родитель следящих детей подождал их и завершился последним
        free(tokens[i]);
    }
    free(tokens);
    fclose(fp);
    return 0;
}

void Split(char *string, char *delimiters, char ***tokens, int *tokensCount) {
    char *pointer_of_token = NULL;
    pointer_of_token = strtok(string, delimiters);
    while (pointer_of_token != NULL) {
        strcpy((*tokens)[*tokensCount], pointer_of_token);//tokensCount-ый элемент разыменованного указателя на массив токенов становится найденным словом
        (*tokensCount)++;
        pointer_of_token = strtok(NULL, delimiters);
    }
}