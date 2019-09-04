#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define N 15 // 遺伝子長(配列)
#define M 15 // 個体数
#define T 100 // Generation
#define Pc 0.30
#define Pm 0.10

struct genotype {
	int gene[N];
	float fitness;
};

float evaluation(int *a) {
	int count = 0;

	for(int i = 0; i < N; i++) {
		count += a[i];
	}
	return((float)count);
}

int flip(float prob) {
	float x = (float)random() / RAND_MAX;

	if(x < prob)
		return 1;
	else
		return 0;
}

void one_point_crossover(struct genotype *ind) {
	int i, ia, ib;
	int j;
	int c;
	int test[M];
	int temp[N];
	int r;

	for(i=0; i<M; i++) test[i] = 0;

	ia = ib = 0;
	for(i = 0; i < M / 2; i++) {
		for(; test[ia] == 1; ia = (ia + 1) % M);
		test[ia] = 1;
		r = random() % (M - 2 * i) + 1;
		while(r > 0) {
			ib=(ib + 1) % M;
			for(; test[ib] == 1; ib = (ib + 1) % M);
			r--;
		}
		test[ib] = 1;
		if(flip(Pc)) {
			c = random() % N;
			for(j = 0; j < c; j++) {
				temp[j] = ind[ia].gene[j];
				ind[ia].gene[j] = ind[ib].gene[j];
				ind[ib].gene[j] = temp[j];
			}
		}
	}
}

void mutation(struct genotype *ind) {
	for(int i = 0; i < M; i++)
		for(int j = 0; j < N; j++)
			if(flip(Pm)) {
				ind[i].gene[j] = (ind[i].gene[j] + 1) % 2;
			}
}

void roulette_selection(struct genotype *ind) {
	int h;
	float total_fitness;
	float dart;
	float wheel;
	struct genotype ind_new[M];

	total_fitness = 0;
	for(int i = 0; i < M; i++)
		total_fitness += ind[i].fitness;

	for(int i = 0; i < M; i++) {
		dart = (float)random() / RAND_MAX;
		h = 0;
		wheel = ind[h].fitness / total_fitness;
		while(dart > wheel && h < M - 1) {
			h++;
			wheel += ind[h].fitness / total_fitness;
		}
		ind_new[i] = ind[h];
	}

	for(int i = 0; i < M; i++) {
		ind[i] = ind_new[i];
	}

}

void print_process(struct genotype *ind, int generation) {
	float max_fit, min_fit, avg_fit;

	printf("\nGeneration: %d\n", generation);
	for(int i = 0; i < M; i++) {
		printf("%d: ", i);
		for(int j = 0; j < N; j++) {
			if(ind[i].gene[j] == 0) printf("%c", ' ');
			else printf("%c", '*');
		}
		printf(" : %.0f\n", ind[i].fitness);
	}

	max_fit = min_fit = ind[0].fitness;
	avg_fit = ind[0].fitness / M;
	for(int i = 1; i < M; i++) {
		if(max_fit < ind[i].fitness) max_fit = ind[i].fitness;
		if(min_fit > ind[i].fitness) min_fit = ind[i].fitness;
		avg_fit += ind[i].fitness / M;
	}
	printf("max: %.2f  min: %.2f  avg: %.2f\n", max_fit, min_fit, avg_fit);
}


int main(int argc, char *argv[]) {
	struct genotype individual[M];

	if(argc < 2) { // 引数与えなかった時
		printf("Usage: %s [SEED_NUMBER]\n", argv[0]);
		exit(1);
	}
	else {
		srandom(atoi(argv[1]));
	}

	// ステップ1 (0世代目)
	for(int i = 0; i < M; i++) {
		for(int j = 0; j < N; j++) {
			individual[i].gene[j] = flip(0.5);
		}
		individual[i].fitness = evaluation(individual[i].gene);
	}
	print_process(individual, 0);

	// エリートの保存
	int fit = 0;
	for(int i = 0; i < M; i++) {
		if(fit < individual[i].fitness)
			fit = individual[i].fitness;
	}
	int elite[N];
	for(int i = 0; i < M; i++) {
		if(fit == individual[i].fitness) {
			for(int j = 0; j < N; j++) {
				elite[j] = individual[i].gene[j];
			}
			break;
		}
	}

	// ステップ2 (1 ~ T世代目)
	for(int t = 1; t <= T; t++) {
		one_point_crossover(individual);
		mutation(individual);
		for(int i = 0; i < M; i++) {
			individual[i].fitness = evaluation(individual[i].gene);
		}
		roulette_selection(individual);

		// 新世代から劣等生を選択し，旧世代のエリートの代入
		fit = N;
		for(int i = 0; i < M; i++) {
			if(fit > individual[i].fitness)
				fit = individual[i].fitness;
		}
		// printf("%d世代の優等生のfit = %d", t-1, (int)evaluation(elite));
		// printf("\n%d世代の劣等生のfit = %d\n", t, fit);
		for(int i = 0; i < M; ++i) {
			if(fit == individual[i].fitness) {
				for(int j = 0; j < N; j++) {
					individual[i].gene[j] = elite[j];
				}
				// printf("[%d]\n", i);
				break;
			}
		}
		// 適応度の更新
		for(int i = 0; i < M; i++) {
			individual[i].fitness = evaluation(individual[i].gene);
		}
		print_process(individual, t);

		// 新世代のエリートの保存
		fit = 0;
		for(int i = 0; i < M; i++) {
			if(fit < individual[i].fitness)
				fit = individual[i].fitness;
		}
		for(int i = 0; i < M; i++) {
			if(fit == individual[i].fitness) {
				for(int j = 0; j < N; j++) {
					elite[j] = individual[i].gene[j];
				}
				break;
			}
		}
	}

	return 0;
}