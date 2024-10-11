#include <iostream>
#include <vector>
#include <iomanip>
#include "omp.h"
#include "stdlib.h"
#include <new>
#include <thread>
#include <chrono>
//alignas
struct partial_t {
	alignas(std::hardware_destructive_interference_size)
	unsigned value;
};

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
	partial_t* part_sum;

	#pragma omp parallel
	{
		unsigned t = omp_get_thread_num();
		// T - count of threads
		// t - index of thread
		#pragma omp single 
		{
			T = omp_get_num_threads();
			part_sum = (partial_t*)malloc(sizeof(partial_t) * T);
		}
		part_sum[t].value = 0;
		for (int i = t; i < n; i += T) 
		{
			part_sum[t].value += v.at(i);
		}
	}
	for (int i = 0; i < T; ++i) {
		sum += part_sum[i].value;
	}
	free(part_sum);
	return sum;
}

unsigned round_robin(std::vector <unsigned> v, unsigned n) {
	n = v.size();
	partial_t* part_sum;
	int T;
	int sum = 0;
	#pragma omp parallel 
	{
		unsigned t = omp_get_thread_num();
		#pragma omp single 
		{
			T = omp_get_num_threads();
			part_sum = (partial_t*)malloc(sizeof(partial_t) * T);
		}
		part_sum[t].value = 0;
		int s = n / T;
		int b = n % T;
		
		if (t < b)
			b = (s+1) * t;
		else
			b += s * t;

		int e = b + s;
		
		for (int i = b; i < e; i++)
		{
			part_sum[t].value += v[i];
		}
	}
	for (int j = 0; j < T; j++)
	{
		sum += part_sum[j].value;
	}
	free(part_sum);
	return sum;
}

// Эпоха - это фиксированная дата. Иногда это 1970-й год, 1980-й год и др.
int main() {
	
	std::vector <unsigned> vec(1 << 28, 3);
	auto t0 = std::chrono::steady_clock::now();
	std::cout << sum_round_robin(vec, vec.size()) << "\n";
	auto t1 = std::chrono::steady_clock::now();
	std::cout << "Round robin " << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	t0 = std::chrono::steady_clock::now();
	std::cout << sum_omp_reduce(vec, vec.size()) << "\n";
	t1 = std::chrono::steady_clock::now();
	std::cout << "Omp reduce " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	t0 = std::chrono::steady_clock::now();
	std::cout << sum(vec, vec.size()) << "\n";
	t1 = std::chrono::steady_clock::now();
	std::cout << "Sum " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	t0 = std::chrono::steady_clock::now();
	std::cout << round_robin(vec, vec.size()) << "\n";
	t1 = std::chrono::steady_clock::now();
	std::cout << "Sum with local reading " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	//std::cout << "sum(vec, n) = " << std::hex << sum(vec, vec.size()) << "\n";
	//std::cout << "sum_omp_reduce(v, n) = " << std::hex << sum_omp_reduce(vec, vec.size()) << "\n";
	//std::cout << "sum_round_robin(v, n) = " << sum_round_robin(vec, vec.size());
	
	return 0;
}

