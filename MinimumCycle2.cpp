#include <xutility>
#include <ctime>
#include <stdio.h>
#include <omp.h>
#include <queue>
#include <vector>
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

vector<int> path(int s) {
	queue<int> q;
	vector<bool> used(N);
	vector<int> d(N), p(N);
	bool stop = false;
	q.push(s);
	used[s] = true;
	p[s] = -1;
	while (!q.empty() && !stop) {
		int v = q.front();
		q.pop();
		for (int i = 0; i < N && !stop; ++i) {
			if (orgraph[v][i]) {
				if (!used[i]) {
					used[i] = true;
					q.push(i);
					d[i] = d[v] + 1;
					p[i] = v;
				} else if (i == s) {
					stop = true;
				}
			}
		}
	}

	vector<int> r;
	for (int i = 0; i < N; i++)
		if (orgraph[i][s]) r.push_back(i);

	vector<int> path;
	int minPathLen = N + 1;
	if (!r.empty()) {
		for (int i = 0; i < r.size(); i++) {
			int to = r[i];
			if (used[to]) {
				path.clear();
				for (int v = to; v != -1; v = p[v])
					path.push_back(v);
				reverse(path.begin(), path.end());
				path.push_back(s);
			}
		}
	}
	return path;
}

int main() {

	init();
	print();

	long double clockStart, clockStop;
	clockStart = omp_get_wtime();

	vector<int> minPath, p;
	int minPathLen = N + 1;

	#pragma omp parallel for private(p) shared(minPathLen, minPath, i) num_threads(NUM_THREADS)
	for (int i = 0; i < N && minPathLen > 4; i++) {
		p = path(i);
		if (p.size() > 0) {
			#pragma omp critical (minpath)
			{
				if (minPathLen > p.size()) {
					minPathLen = p.size();
					minPath = p;
				}
			}
		}
	}

	clockStop = omp_get_wtime();

	if (0 < minPathLen < N + 1) {
		for (int j = 0; j < minPath.size(); ++j)
			printf("%d ", minPath[j]);
	}

	printf("\n\nSeconds: %f\n", clockStop - clockStart);

	printf("\n>");
	getchar();
	return 0;
}