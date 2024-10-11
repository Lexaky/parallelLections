#include <iostream>
#include <vector>
#include <iomanip>
#include "omp.h"
#include "stdlib.h"

//В ++ реализовано std::harware_constructive_interference_size и
//std::harware_destructive_interference_size

//Выравнивание элементов данных
/*
	Если данные имеют некоторый виртуальный адрес A (целое число)
	Адрес должен делиться на некоторое число.
	Выравнивание данных по D - значит, что A mod D = 0
	В случае двоичной арифметики:
	A mod B^d <=> A выравнено по d-символьной границе

	Одним из способов выравнивания данных является создание
	собственной структуры (type punning)
	struct some_struct {
		union {
			unsigned value;
			char padding_[64];
			double c;
		}
	}
	some_struct str;
	Работа объединения:
	[*|*|*|*|*|*|*|*]
	|unsigned|000000| или
	|double     |000| или
	|char[64]       | весь массив
*/

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

//unsigned round_robin(std::vector <unsigned> v, unsigned n) {
//	n = v.size();
//#pragma omp parallel 
//	{
//		int T = omp_get_num_threads();
//		int s = n / T;
//		int b = n % T;
//		unsigned t = omp_get_thread_num();
//		if (t < b)
//			b = ++s * t;
//		else
//			b += s * t;
//		int e = b + s;
//		for (int i_start = b; i_start < e; i_start++)
//		{
//			//part_sum += v[i]
//		}
//		//sum = part_sum[0...t]
//	}
//	
//}
// 
// Эпоха - это фиксированная дата. Иногда это 1970-й год, 1980-й год и др.
int main() {
	
	
	std::vector <unsigned> vec(1 << 28, 3);
	double t0 = omp_get_wtime();
	std::cout << sum_round_robin(vec, vec.size()) << "\n";
	std::cout << "Took " << (omp_get_wtime() - t0)*1000 << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	t0 = omp_get_wtime();
	std::cout << sum_omp_reduce(vec, vec.size()) << "\n";
	std::cout << "Took " << (omp_get_wtime() - t0) * 1000 << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	t0 = omp_get_wtime();
	std::cout << sum(vec, vec.size()) << "\n";
	std::cout << "Took " << (omp_get_wtime() - t0) * 1000 << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	//std::cout << "sum(vec, n) = " << std::hex << sum(vec, vec.size()) << "\n";
	//std::cout << "sum_omp_reduce(v, n) = " << std::hex << sum_omp_reduce(vec, vec.size()) << "\n";
	//std::cout << "sum_round_robin(v, n) = " << sum_round_robin(vec, vec.size());
	
	return 0;
}

