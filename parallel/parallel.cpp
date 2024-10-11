#include <iostream>
#include <vector>
#include <iomanip>
#include "omp.h"
#include "stdlib.h"

unsigned sum_omp_reduce(std::vector<unsigned> v, unsigned n) {
	unsigned sum = 0;
#pragma omp parallel for reduction (+ : sum)
	for (int i = 0; i < n; i++) {
		sum += v.at(i);
	}
	return sum;
}

unsigned sum(std::vector<unsigned> v, unsigned n) {
	unsigned sum = 0;
	for (int i = 0; i < n; i++) {
		sum += v.at(i);
	}
	return sum;
}

//round_robin algorithm
unsigned sum_round_robin(std::vector<unsigned> v, unsigned n) {
	unsigned sum = 0;
	unsigned T = 0;
	// Введём массив сумм каждого потока
	unsigned* part_sum;
	
	#pragma omp parallel
	{
		unsigned t = omp_get_thread_num();
		// T - count of threads
		// t - index of thread
		#pragma omp single 
		{
			T = omp_get_num_threads();
			part_sum = (unsigned*)calloc(sizeof v[0], T);
		}
		for (int i = t; i < n; i += T) {
			part_sum[t] += v.at(i);
		}
	}
	sum = part_sum[0];
	for (int i = 1; i < T; ++i) {
		sum += part_sum[i];
	}
	free(part_sum);
	return sum;
}

int main() {

	std::vector <unsigned> vec(1 << 24, 12);
	std::cout << "sum(vec, n) = " << std::hex << sum(vec, vec.size()) << "\n";
	std::cout << "sum_omp_reduce(v, n) = " << std::hex << sum_omp_reduce(vec, vec.size()) << "\n";
	std::cout << "sum_round_robin(v, n) = " << sum_round_robin(vec, vec.size());
	return 0;
}