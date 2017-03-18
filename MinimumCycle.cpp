#include <xutility>
#include <ctime>
#include <stdio.h>
#include <omp.h>

using namespace std;

const int NUM_THREADS = 4;
const int N = 500;
//const int N = 10;
const int MAX_C = INT_MAX;
int orgraph[N][N] = {};
//int orgraph[N][N] = {
//	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
//	{ 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 }, 
//	{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }, 
//	{ 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 },
//	{ 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 }, 
//	{ 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 }, 
//	{ 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 }, 
//	{ 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
//	{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 }, 
//	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }
//};

struct Path {
	int v[N + 1];
	int c;
};

void init() {
	srand(time(0));
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (i != j && orgraph[j][i] != 1 && orgraph[i][j] != 1)
				orgraph[i][j] = rand() % 11 == 0 ? 1 : 0;
}

void print() {
	FILE *h;
	if ((h = fopen("adjacency_matrix.txt", "w")) == NULL)
		printf("Error\n");
	else {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++)
				fprintf(h, "%d, ", orgraph[i][j]);
			fprintf(h, "\n");
		}
		fclose(h);
	}
}

bool exists(int v, int c[], int count)
{
	for (int i = 0; i < count; i++)
		if (c[i] == v) return true;
	return false;
}

void dijkstra(int start, int d_result[]) {
	bool u[N];
	int e = 0, i, d[N] = {};

	fill_n(u, N, false);
	fill_n(d, N, MAX_C);
	d[start] = 0;

	for (e = 0; e < N; e++)
	{
		int v = 0;
		while (u[v]) v++;
		for (i = v + 1; i < N; i++)
			if (!u[i] && d[i] < d[v]) v = i;
		u[v] = true;
		for (i = 0; i < N; i++)
			if (orgraph[v][i])
				d[i] = min(d[i], d[v] + 1);
	}
	memcpy(d_result, d, sizeof(int) * N);
}

int imin(int a[], int count, int d[]) {
	int m = 0;
	for (int i = 1; i < count; i++)
		if (d[a[i]] < d[a[m]]) m = i;
	return m;
}

Path restore(int start) {
	int d[N] = {};
	dijkstra(start, d);

	int s[N] = {}, scount = 0;
	int v = start;
	do {
		int p[N] = {}, pcount = 0;
		for (int i = 0; i < N; i++)
			if (orgraph[i][v] && d[i] < MAX_C) p[pcount++] = i;

		if (!(pcount > 0)) break;

		v = p[0];
		for (int i = 1; i < pcount; i++)
			if (d[p[i]] < d[v]) v = p[i];

		s[scount++] = v;
	} while (v != start && scount < N);
	
	Path path;
	path.c = 0;
	memcpy(path.v, s, sizeof(int) * scount);
	path.c = scount;

	return path;
}

// Поиск цикла минимальной длины

int main() {

	init();
	print();

	long double clockStart, clockStop;
	clockStart = omp_get_wtime();

	int k = 0, i;
	Path m, c;
	do {
		m = restore(k++);
	} while (m.c < 3);

	#pragma omp parallel for private(c) shared(m, i) num_threads(NUM_THREADS)
	for (i = k + 1; i < N; i++)
	{
		c = restore(i);
		#pragma omp critical (minvalue)
		{
			if (!(c.c < 3) && c.c < m.c && c.v[0] == c.v[m.c - 1]) {
				memcpy(m.v, c.v, sizeof(int) * c.c);
				m.c = c.c;
			}
		}
	}

	clockStop = omp_get_wtime();

	if (m.c < 3) printf("Cycle not found\n");
	else {
		for (int i = m.c - 1; i >= 0; i--)
			printf("%d ", m.v[i]);
		printf("%d", m.v[m.c - 1]);
	}

	printf("\n\nSeconds: %f\n", clockStop - clockStart);
	printf("\n>");
	getchar();
	return 0;
}