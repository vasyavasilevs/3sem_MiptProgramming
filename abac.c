#include <stdio.h>
#include <string.h>
#include <malloc.h>
/*
 * Необходимо сгенерировать последовательность
строк вида (выделить память, сгенерировать, а
затем вывести):
• a
• aba
• abacaba
• abacabadabacaba
• ...
void GenerateString(int n, char* string);
 */

void GenerateString(int n, char* string) {
    int i = 0;
    char* current;
    current = malloc(sizeof(char) * (1 << n));
    for (i = 1; i <= n; i++) {
        printf("%d) %s\n", i, string);
        strcpy(current, string);
        string[strlen(string)] = 'a' + i;
        strcat(string, current);
    }
}

int main() {
    int num_of_iterations = 0;
    printf("Введите количество итераций: ");
    scanf("%d", &num_of_iterations);
    char* string;
    string = malloc(sizeof(char) * (1 << num_of_iterations));
    string[0] = 'a';
    GenerateString(num_of_iterations, string);
    free(string);
    return 0;
}

