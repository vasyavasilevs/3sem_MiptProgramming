#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

void Split(char *string, char *delimiters, char ***tokens, int *tokensCount);

#define max_string_size 1000
#define max_token_size 1000
#define number_of_tokens 1000
#define TIMEOUT 5

int main () {
    int i = 0, tokens_count = 0, counter_of_strings = 0, child_id = 0;
    char stdin_delimiters[] = {' ', '\n', '\0'};
    char stdin_string[max_string_size] = {0};
    FILE *fp; //fp - имя указателя на файл
    fp = fopen("commands.txt", "r");

    char **tokens = (char **) malloc(sizeof(char *) * number_of_tokens);//массив токенов (слов)
    for (i = 0; i < number_of_tokens; i++) {
        tokens[i] = (char *) calloc(max_token_size, sizeof(char));
    } //выделение памяти под каждый токен (слово)

    fgets(stdin_string, max_string_size, fp);
    counter_of_strings = atoi(stdin_string); //преобразуем строку в число, с которого эта строка начинается (= 3)
 
    for (i = 0; i < counter_of_strings; i++) {
        fgets(stdin_string, max_string_size, fp);
        pid_t pid = fork();
        if (pid == 0) {
	    child_id = getpid();
            Split((char *)stdin_string, (char *)stdin_delimiters, &tokens, &tokens_count);
            sleep(atoi(tokens[0])); //спит столько, сколько указано в начале каждой строки
            execvp(tokens[1], tokens + 1);
            exit(0);
        }
    }
    for (i = 0; i < counter_of_strings; i++) {
        sleep(atoi(tokens[0]) + TIMEOUT);//спит столько, сколько ребенок, + положенный timeout, в течение которого дитя работает
        int result = 0;
        result = waitpid(-1, NULL, WNOHANG);
        if(result > 0)
            break; //если результат больше нуля, то ребенок завершился за timeout
        else 
	    kill(child_id, SIGTERM); //принудительно завершить дочерний процесс, вышедший за рамки timeout
    }
    for (i = 0; i < number_of_tokens; i++) {
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
    (*tokens)[*tokensCount] = NULL;
}
