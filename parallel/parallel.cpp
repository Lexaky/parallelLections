#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include <chrono>
#include "num_threads.h"
#include <fstream>

static unsigned thread_no = std::thread::hardware_concurrency();

unsigned sum_cpp_mutex(std::vector <unsigned> v, unsigned v_size) 
{
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
	auto sr = run_experiment(sum_cpp_mutex, 1, 10000000);

	std::ofstream os("file.csv", std::ios_base::out);
	if (os.is_open()) {
		to_csv(os, sr);
		os.close();
	}


	return 0;
}

