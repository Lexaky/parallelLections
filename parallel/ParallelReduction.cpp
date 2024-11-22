#include <iostream>
#include "own_barrier_latch.h"
#include "num_threads.h"
// Редукция - это отображение большего числа элементов в меньшее: S^n -> S^m, m < n
// Параллельная редукция - это когда мы делаем это параллельно (как с частичными суммами
// o - операция арности k >= 2
// e - нейтральный элемент (необходимый для перехода)
// ассоциативность o

void part_sum() {
	size_t m = get_num_threads();
	for (size_t neighbor = 1, next = 2; neighbor < m; neighbor = next, next += next) {
		if (t % next == 0 && t + neighbor < m) {
			partitial_sums[t] += partitial_sums[t + neighbor];
		}
	// Где-то здесь необходимо использовать барьер. Самостоятельно подумать, мб где-то здесь его применить, либо в if
	// либо ещё где-то
	}
	return paritital_sum[0];
}