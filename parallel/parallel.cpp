#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include <chrono>

unsigned sum_cpp_mutex(std::vector <unsigned> v) 
{
	unsigned sum = 0;
	unsigned v_size = v.size();
	std::mutex mtx;
	auto worker = [&v, &sum, &mtx, &v_size](unsigned t) {
		unsigned local_sum = 0;
		const unsigned threadsCount = std::thread::hardware_concurrency();
		size_t s = v_size / threadsCount,
			   b = v_size % threadsCount;
		t < b ? b = ++s * t : b += s * t;
		const size_t e = b + s;
		for (size_t i = b; i < e; ++i) {
			local_sum += v.at(i);
		}
		mtx.lock();
		sum += local_sum;
		mtx.unlock();
	};
	std::vector <std::thread> w(std::thread::hardware_concurrency());
	for (unsigned t = 0; t < std::thread::hardware_concurrency(); t++) {
		w.at(t) = std::thread(worker, t);
	}
	for (auto& thr : w) {
		thr.join();
	}
	return sum;
}

int main() {
	
	std::vector <unsigned> vec(1 << 28, 3);
	auto t0 = std::chrono::steady_clock::now();
	std::cout << sum_cpp_mutex(vec) << "\n";
	auto t1 = std::chrono::steady_clock::now();
	std::cout << "sum_cpp_mutex:  " << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() << " ms\n";
	std::fill_n(vec.begin(), vec.size(), 3);
	
	return 0;
}

