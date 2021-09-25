#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max_string_size 1000
#define max_delimiter_size 1000
#define max_token_size 1000
#define number_of_tokens 1000

void Split(char *string, char *delimiters, char ***tokens, int *tokensCount);

int main() {
    int i = 0, tokens_count = 0;
    char stdin_string[max_string_size] = {0}, stdin_delimiters[max_delimiter_size] = {0};
    char **tokens = (char **)malloc(sizeof(char *) * number_of_tokens);//массив токенов
    printf("Enter a string (including delimiters): ");
    gets(stdin_string);
    printf("Enter a delimiter: ");
    gets(stdin_delimiters);
    for (i = 0; i < number_of_tokens; i++) {
        tokens[i] = malloc(sizeof(char) * max_token_size);
    }//выделение памяти под каждый токен
    Split((char *)stdin_string, (char *)stdin_delimiters, &tokens, &tokens_count);
    for (i = 0; i < tokens_count; i++) {
        printf("%d word: %s\n", i + 1, tokens[i]);
    }
    for (i = 0; i < number_of_tokens; i++) {
        free(tokens[i]);
    }
    free(tokens);
    return 0;
}

void Split(char *string, char *delimiters, char ***tokens, int *tokensCount) {
    char *pointer_of_token = strtok(string, delimiters);
    while (pointer_of_token != NULL) {
        strcpy((*tokens)[*tokensCount], pointer_of_token);//tokensCount-ый элемент разыменованного указателя на массив токенов становится найденным словом
        (*tokensCount)++;
        pointer_of_token = strtok(NULL, delimiters);
    }
}
