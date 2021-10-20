#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

/*
run with gcc -Wall -o segments_and_time segments_and_time.c -lpthread -lm -O3
*/

/*
Для проверки уменьшения временнЫх затрат на работу программы при использовании нитей
можно менять в define их количество. Правда, с какого-то большого их
значения (порядка 1e4 точно) time of working наоборот начинает увеличиваться, скорее всего из-за того, что 
растущее время, затрачиваемое на создание этих нитей, перекрывает эффект ускорения, появляющийся из-за разделения труда,
так что лучшее - враг хорошего.
*/

/*
1 нить:
Time of working = 0.104414
The sum of all experimental data = 5049815887.000000
Math expectation = 50.498159
Dispersion = 833.218694

2 нити:
Time of working = 0.054595
The sum of all experimental data = 5050051896.000000
Math expectation = 50.500519
Dispersion = 833.282213

3 нити:
Time of working = 0.037456
The sum of all experimental data = 5049854380.000000
Math expectation = 50.498544
Dispersion = 833.161184

4 нити:

...

100 нитей:
Time of working = 0.017188
The sum of all experimental data = 5050176237.000000
Math expectation = 50.501762
Dispersion = 833.189989

1000 нитей (уже замедление по сравнению с предыдущим):
Time of working = 0.027118
The sum of all experimental data = 5050062512.000000
Math expectation = 50.500625
Dispersion = 833.212933

10000 нитей (даже больше времени работы с одной нитью):
Time of working = 0.179901
The sum of all experimental data = 5050435245.000000
Math expectation = 50.504352
Dispersion = 833.140530
*/

#define NUMBER_OF_THREADS 1
#define ARRAY_SIZE ((int)1e8)

#define KEY "%lf"
float *array = NULL;
double segment_sum[NUMBER_OF_THREADS];
double segment_sum_square[NUMBER_OF_THREADS];

typedef struct {
	int begin;
	int end;
	int index;
} Segment;

void* FindSums (void* arg) {
	Segment segment = *((Segment *)arg);//разыменование указателя на структуру iго объекта segments, приведенного к типу Segment *
	int i = 0;
	segment_sum[segment.index] = 0;
	segment_sum_square[segment.index] = 0;
	for (i = segment.begin; i < segment.end; i++) {
		segment_sum[segment.index] += array[i];
		segment_sum_square[segment.index] += pow(array[i], 2);
	}
    return NULL;
}

int main (int argc, char* argv[]) {
	array = (float *) calloc(ARRAY_SIZE, sizeof(float));
	int i = 0;

	srand(time(NULL));
    for (i = 0; i < ARRAY_SIZE; i++) {
		array[i] = (1 + rand()%100);
	}

	struct timespec start, finish;
	double elapsed = 0;
	clock_gettime(CLOCK_MONOTONIC, &start);

	Segment segments[NUMBER_OF_THREADS];
	int segment_size = ARRAY_SIZE/NUMBER_OF_THREADS;//делим массив на одинаковые части - сегменты (по кол-ву нитей)
	
	//заполнение сегментов началом, концом и индексами
	for (i = 0; i < NUMBER_OF_THREADS; i++) {
		segments[i].index = i;
		segments[i].begin = segment_size * i;
		if (i != NUMBER_OF_THREADS - 1) {
			segments[i].end = segments[i].begin + segment_size;
		} else {
			segments[i].end = ARRAY_SIZE;//не забыть последний элемент, из-за округления segment_size может потеряться
		}
	}
	
	pthread_t array_thread_id[NUMBER_OF_THREADS];
	
    int result = 0;
    //множим нити и отправляем каждый сегмент на обработку в его нить
	for (i = 0; i < NUMBER_OF_THREADS; i++) {
		result = pthread_create(&(array_thread_id[i]), (pthread_attr_t *)NULL, FindSums, &(segments[i]));
        if (result != 0) {
			printf ("Error on thread create, return value = %d\n", result);
			exit(-1);
    	}
	}

	//чтобы всех дождались
	for (i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_join(array_thread_id[i], NULL);
	}

	double sum = 0, sum_square = 0, math_expectation = 0, math_expectation_of_square = 0;
	
	//сложение посчитанных значений для каждого сегмента
	for (i = 0; i < NUMBER_OF_THREADS; i++) {
		sum += segment_sum[i];
		sum_square += segment_sum_square[i];
	}
	
	math_expectation = sum / ARRAY_SIZE;//е от кси (мат.ожид)
	math_expectation_of_square = sum_square / ARRAY_SIZE;//е от кси-квадрат

	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec); //время в секундах
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1e9; //хвост времени в наносекундах
	
	printf("Time of working = " KEY "\n", elapsed);
	printf("The sum of all experimental data = " KEY "\n", sum);
	printf("Math expectation = " KEY "\n", math_expectation);
	printf("Dispersion = " KEY "\n", math_expectation_of_square - pow(math_expectation, 2));

	free(array);
    return 0;
}
