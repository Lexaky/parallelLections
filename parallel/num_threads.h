#pragma once
#include <concepts>
#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>
#include <numeric>
#include <iostream>

void set_num_threads(unsigned T);
unsigned get_num_threads();

struct scalability_result {
    unsigned result; // Это должна быть сумма для отслеживания результата значения функции
    double t, s, e; // Ускорение и эффективность
};




//Доделать перегрузки так, чтобы invocable был const unsigned *v и size_t n, причём для каждой функции суммы
//сделать перегрузки методов, которые принимают векторы для суммирования, чтобы они принимали си-шные массивы
template <std::invocable<const std::vector<unsigned>&, size_t> Functor>
std::vector<scalability_result> run_experiment(Functor f, unsigned v_size_min, unsigned v_size_max) {
    std::vector<unsigned> v(v_size_max - v_size_min);
    std::iota(v.begin(), v.end(), v_size_min);
    unsigned P = get_num_threads();

    std::vector<scalability_result> scale(P);

    for (unsigned T = 1; T <= P; ++T) {
        set_num_threads(T);
        auto t_start = std::chrono::steady_clock::now();
        scale.at(T-1).result = f(v, v.size());
        auto t_end = std::chrono::steady_clock::now();
        scale.at(T-1).t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count());
        scale.at(T-1).s = static_cast<double>(scale.at(0).t) / static_cast<double>(scale.at(T-1).t);
        scale.at(T-1).e = static_cast<double>(scale.at(T-1).s) /T;
    }
    v.clear();

    return scale;
}

/*
	1) Инициализировать вектор std::vector <scalability_result>
	   значениями от v_min до v_max(не включительно) (по порядку) - их количество v_max - v_min
	2) Выполнить функцию f над v размером v_max - v_min
	3) Отобразить результаты в результирующий вектор значениями 
	f, t1-t0
	4) вернуть результирующий вектор
*/