#pragma once
#include <concepts>
void set_num_threads(unsigned T);
unsigned get_num_threads();

struct scalability_result {
	unsigned result;
	double s, e; // Ускорение и эффективность
	//s = t1/tT, где t1 - время выполнения метода одним потоком
	//e = S/T
};
//Переключиться на C++20 для #include <concept>
template <std::invocable <const unsigned*, size_t> Functor>
std::vector<scalability_result> run_experiment(Functor f, unsigned v_size_min, unsigned v_size_max);
/*
	1) Инициализировать вектор std::vector <scalability_result>
	   значениями от v_min до v_max(не включительно) (по порядку) - их количество v_max - v_min
	2) Выполнить функцию f над v размером v_max - v_min
	3) Отобразить результаты в результирующий вектор значениями 
	f, t1-t0
	4) вернуть результирующий вектор
*/