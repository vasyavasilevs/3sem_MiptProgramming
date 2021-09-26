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
void GenerateString(int n, char *string);
 */

void GenerateString(int n, char *string) {
    int i = 0;
    char *current = NULL;
    current = (char *) malloc(sizeof(char) * (1 << n));
    for (i = 0; i < n; i++) {
	strcpy(current, string); //current становится string
        string[strlen(string)] = 'a' + i; //в конец string приписывается след. буква алфавита
        strcat(string, current); //к string в конец приписывается current aka предыдущий string
	printf("%d) %s\n", i + 1, string); //печатается новый string
    }
    free(current);
}

int main() {
    int num_of_iterations = 0;
    printf("Введите количество итераций: ");
    scanf("%d", &num_of_iterations);
    char *str;
    str = (char *) malloc(sizeof(char) * (1 << num_of_iterations));
    str[0] = '\0';
    GenerateString(num_of_iterations, str);
    free(str);
    return 0;
}
// TODO: позвали дважды malloc, но всего 1 раз free
//можно было в предыдущей версии сдвигать на n + 1, чтобы избежать "double free or corruption (out), Аварийный останов (стек памяти сброшен на диск)", но так вроде правильнее
