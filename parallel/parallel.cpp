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
		// T - max count of threads
		// t - index of thread
		#pragma omp single 
		{
			T = omp_get_num_threads(); // Получили максимальное кол-во потоков
			part_sum = (partial_t*)malloc(sizeof(partial_t) * T); // Выделили память под частичные суммы
		}
		part_sum[t].value = 0; // инициализация каждой частичной суммы
		for (int i = t; i < n; i += T) 
		{
			part_sum[t].value += v.at(i); // каждый поток t обработает свой индекс (прыжками)
		}
		/*
			Т.е. например вектор [1, 2, 3, 4, 5, 6, 7, 8, 9, 10] и 4 потока:
			Поток 0:
			v[0] -> v[0+T, T = 4] -> v[0 + 2T, T = 4]
			part_sum[0] = v[0] + v[4] + v[8]
			Поток 1:
			v[1] -> v[1+T, T = 4] -> v[1 + 2T, T = 4]
			part_sum[1] = v[1] + v[5] + v[9]
			Поток 2:
			v[2] -> v[2+T, T = 4] -> v[1 + 2T, T = 4]
			part_sum[2] = v[2] + v[6] + [1+2T = 10 > n (это не учитывается по условиям цикла)]
			Поток 3:
			v[3] -> v[3+T, T = 4] -> v[1 + 2T, T = 4]
			part_sum[3] = v[3] + v[7]
			Поток 4:
			v[4] -> v[4 + T, T = 4]
			part_sum[4] = v[4] + v[8]
			
									Потоки:  0	   1	  2	    3
			Итого были обработаны элементы: v[0], v[1], v[2], v[3];
											v[4], v[5], v[6], v[7];
											v[8], v[9]
			Весь массив просуммирован в частичные суммы, далее суммируются частичные суммы для получения результата
		*/
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
		int s, b, e;
		#pragma omp single 
		{
			T = omp_get_num_threads();
			part_sum = (partial_t*)malloc(sizeof(partial_t) * T);
			s = n / T; // Вычисляются количество элементов, которые ТОЧНО обработает каждый поток
			b = n % T; // Эти элементы остаточные, они распределятся между первыми потоками (их кол-во зависит от b)
		}

		part_sum[t].value = 0;

		// Количество первых потоков, которые будут обрабатывать остаточные элементы вычисляются далее по условиям
		
		if (t < b)
		{
			b = (s + 1) * t;
			e = b + s + 1; // До какого индекса обрабатываются потоки
		}
		else
		{
			b = b + s * t;
			e = b + s;
		}
		
		/*
			Т.е. если номер потока t < количества остаточных элементов, то поток
			возьмёт на 1 элемент больше, остальные потоки.
			Если его номер больше, либо равен количеству остаточных элементов, то поток
			возьмёт то количество элементов, которые лежат в s (т.е. ТОЧНОЕ количество)
		*/

		for (int i = b; i < e; i++)
		{
			part_sum[t].value += v[i];
		}
		/*
		Пример: вектор v = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10], потоков 4
		Вычисляется S = v.size()/4 = 2 - это значит, что каждый поток обработает ТОЧНО 2 элемента массива v
		Далее b = v.size() % 4 = 2 - это значит, что осталось 2 элемента, которые не обработаются
		и это логично, потому что если каждый поток из 4 обработает по 2 элемента ТОЧНО, то будет обработано всего 8 элементов

		Далее смотрим на условие (t < b)
		У нас есть потоки 0, 1, 2, 3 (4 штуки всего).
		Потоки t = 0, t = 1 попадают под это условие, т.е. для них вычисляем такие начальные элементы:
		Поток 0: b = (2 + 1) * 0 = 0
				 e = b + s + 1 = 0 + 2 + 1 = 3
				 Это значит, что поток обработает с 0 элемента до 3 не включительно (v[0], v[1], v[2])
		Поток 1: b = (2 + 1) * 1 = 3
				 e = b + s + 1 = 3 + 2 + 1 = 6
				 Это значит, что поток обработает с 3 до 6 элемента не включительно (v[3], v[4], v[5])
		Остальные потоки не обрабатывают оставшиеся элементы:
		Поток 2: b = 2 + 2 * 2 = 6
				 e = 6 + 2 = 8
				 Это значит, что поток обработает с 6 до 8 элемента не включительно - т.е. 2 (v[6], v[7])
		Поток 3: b = 2 + 2 * 3 = 8
				 e = 8 + 2 = 10
				 Это значит, что поток обработает с 8 до 10 элемента не включительно - т.е. 2 (v[8], v[9])
		Т.о. обработан весь массив.
		*/

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
	std::cout << "sum(vec, n) = " << std::hex << sum(vec, vec.size()) << "\n";
	std::cout << "sum_omp_reduce(v, n) = " << std::hex << sum_omp_reduce(vec, vec.size()) << "\n";
	std::cout << "sum_round_robin(v, n) = " << sum_round_robin(vec, vec.size());
	
	return 0;
}

