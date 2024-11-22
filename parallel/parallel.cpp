#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include <chrono>
#include "num_threads.h"
#include <fstream>
#include "omp.h"

static unsigned thread_no = std::thread::hardware_concurrency();

struct partial_t {
	alignas(std::hardware_destructive_interference_size) // = 40
		unsigned value;
};



unsigned round_robin(std::vector<unsigned> v, unsigned n) {
	n = v.size();
	partial_t* part_sum;
	unsigned T;
	unsigned sum = 0;
#pragma omp parallel 
	{
		unsigned t = omp_get_thread_num();
		unsigned s, b, e;
#pragma omp single 
		{
			T = omp_get_num_threads();
			part_sum = (partial_t*)malloc(sizeof(partial_t) * T);
			s = n / T;
			b = n % T;
		}

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

		for (unsigned i = b; i < e; i++)
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

