#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include <chrono>
#include "num_threads.h"

static unsigned thread_no = std::thread::hardware_concurrency();

unsigned sum_cpp_mutex(std::vector <unsigned> v) 
{
	unsigned sum = 0;
	unsigned v_size = v.size();
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


int main() {
	
	std::vector <unsigned> vec(1 << 28, 3);
	for (unsigned T = 1; T < std::thread::hardware_concurrency(); T++) {
		omp_set_num_threads(T);
		auto t0 = std::chrono::steady_clock::now();
		std::cout << sum_cpp_mutex(vec) << "\n";
		auto t1 = std::chrono::steady_clock::now();
		std::cout << "sum_cpp_mutex:  " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms, using " << T << " threads\n";
	}
	std::fill_n(vec.begin(), vec.size(), 3);
	
	return 0;
}

