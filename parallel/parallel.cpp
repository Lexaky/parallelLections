#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include <chrono>
#include "num_threads.h"
#include <fstream>
#include "omp.h"
#include "own_barrier_latch.h"
#include <thread>

static unsigned thread_no = std::thread::hardware_concurrency();

struct partial_t {
	alignas(std::hardware_destructive_interference_size) // = 40
		unsigned value;
};



unsigned localization_sum(unsigned * v, unsigned n) {
	
	unsigned T = get_num_threads();
	auto part_sum = std::make_unique<partial_t[]>(T);
	unsigned sum = 0; Barrier br(T);
	auto worker_proc = [&part_sum, T, v, n, &br](unsigned t) {
		unsigned s, b, e;
		s = n / T;
		b = n % T;
		part_sum[t].value = 0;
		if (t < b)
		{
			b = (s + 1) * t;
			e = b + s + 1;
		}
		else
		{
			b = b + s * t;
			e = b + s;
		}
		unsigned m = 0;
		for (unsigned i = b; i < e; i++)
		{
			m += v[i];
		}
		part_sum[t].value = m;
		br.arrive_and_wait();
		for (size_t neighbor = 1, next = 2; neighbor < T; neighbor = next, next += next) {
			if (t % next == 0 && t + neighbor < T) {
				part_sum[t].value += part_sum[t + neighbor].value;
			}
			br.arrive_and_wait();
		}
		
	};

	// Запуск кода суммирования с потоков
	std::vector <std::thread> w(get_num_threads() - 1);
	for (unsigned t = 1; t < get_num_threads(); t++) {
		w.at(t - 1) = std::thread(worker_proc, t);
	}
	worker_proc(0);
	for (auto& thr : w) {
		thr.join();
	}

	return part_sum[0].value;
}

unsigned sum_c_mutex(unsigned* v, unsigned v_size) {
	unsigned sum = 0;
	std::mutex mtx;
	auto worker = [&v, &sum, &mtx, &v_size](unsigned t) {
		unsigned local_sum = 0;
		const unsigned threadsCount = get_num_threads();
		size_t s = v_size / threadsCount,
			b = v_size % threadsCount;
		t < b ? b = ++s * t : b += s * t;
		const size_t e = b + s;
		for (size_t i = b; i < e; ++i) {
			local_sum += v[i];
		}

		{
			std::scoped_lock(mtx);
			sum += local_sum;
		}
	};
	
	std::vector <std::thread> w(get_num_threads() - 1);
	for (unsigned t = 1; t < get_num_threads(); t++) {
		w.at(t - 1) = std::thread(worker, t);
	}
	worker(0);
	for (auto& thr : w) {
		thr.join();
	}
	return sum;
}

unsigned sum_cpp_mutex(std::vector <unsigned> v, unsigned v_size) {
	unsigned sum = 0;
	std::mutex mtx;
	auto worker = [&v, &sum, &mtx, &v_size](unsigned t) {
		unsigned local_sum = 0;
		const unsigned threadsCount = get_num_threads();
		size_t s = v_size / threadsCount,
			   b = v_size % threadsCount;
		t < b ? b = ++s * t : b += s * t;
		const size_t e = b + s;
		for (size_t i = b; i < e; ++i) {
			local_sum += v.at(i);
		}

		{
			std::scoped_lock(mtx);
			sum += local_sum;
		}
	};

	std::vector <std::thread> w(get_num_threads() - 1);
	for (unsigned t = 1; t < get_num_threads(); t++) {
		w.at(t-1) = std::thread(worker, t);
	}
	worker(0);
	for (auto& thr : w) {
		thr.join();
	}
	return sum;
}

void to_csv(std::ostream& io, std::vector<scalability_result> v) {
	io << "N,Result,Time,Speedup,Efficiency\n";
	for (unsigned i = 0; i < v.size(); i++) {
		io << i << ',' << v.at(i).result << ',' << v.at(i).t << ',' << v.at(i).s << ',' << v.at(i).e << '\n';
	}
}

int main() {
	auto sr = run_experiment(sum_cpp_mutex, 1u, 1u<<28);

	std::ofstream os("file.csv", std::ios_base::out);
	if (os.is_open()) {
		to_csv(os, sr);
		os.close();
	}


	return 0;
}

