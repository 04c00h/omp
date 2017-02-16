#include <stdio.h>
#include <omp.h>
#include <climits>
#include <stdlib.h>
#include <ctime>

const int NUM_THREADS = 4;
const int N = 10000;
const int M = 10000;
int matrix[N][M];

//Дана прямоугольная матрица чисел nxm. Найти минимальный среди максимальных элементов каждого столбца
void min_matrix()
{
	long double clockStart, clockStop;
	int i, j, tmpMin, tmpMax;

	clockStart = omp_get_wtime();
	//srand(time(0)); //выкл. для отладки, чтобы значения rand() повторялись

	#pragma omp parallel for private(j) shared(i) num_threads(NUM_THREADS)
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			matrix[i][j] = rand();
		}
	}

	tmpMin = INT_MAX;

	#pragma omp parallel for private(tmpMax, j) shared(tmpMin, i) num_threads(NUM_THREADS)
	for (i = 0; i < N; i++) {
		tmpMax = matrix[i][0];
		for (j = 1; j < M; j++) {
			if (tmpMax < matrix[i][j])
				tmpMax = matrix[i][j];
		}
		#pragma omp critical (minvalue)
		{
		if (tmpMin > tmpMax)
			tmpMin = tmpMax;
		}
	}

	clockStop = omp_get_wtime();
	
	printf("MIN = %d\n", tmpMin);
	printf("Seconds: %f\n", clockStop - clockStart);
}

int main()
{
	min_matrix();

	getchar();
	return 0;
}
