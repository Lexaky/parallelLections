#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include "num_threads.h"
#include <fstream>
#include "own_barrier_latch.h"
#include <thread>
#include "ExperementsRunner.h"
#include "Sums.h"
#include "RandomizeGenerator.h"

// Method to create csv file from vector that contains scalability_result objects
void to_csv(std::ostream& io, std::vector<scalability_result> v) {
	io << "N,Result,Time,Speedup,Efficiency\n";
	for (unsigned i = 0; i < v.size(); i++) {
		io << i << ',' << v.at(i).result << ',' << v.at(i).t << ',' << v.at(i).s << ',' << v.at(i).e << '\n';
	}
}
int main() {
	auto sr = run_experiment(sum_c_mutex, 1u, 1u<<28);

	std::ofstream os("sum_c_mutex.csv", std::ios_base::out);
	if (os.is_open()) {
		to_csv(os, sr);
		os.close();
	}

	auto sr2 = run_experiment(localization_sum, 1u, 1u << 28);
	std::ofstream os2("localization_sum.csv", std::ios_base::out);
	if (os2.is_open()) {
		to_csv(os2, sr2);
		os2.close();
	}

	auto sr3 = run_experiment(sum_omp_reduce, 1u, 1u << 28);
	std::ofstream os3("sum_omp_reduce.csv", std::ios_base::out);
	if (os3.is_open()) {
		to_csv(os3, sr3);
		os3.close();
	}

	auto sr4 = run_experiment(sum, 1u, 1u << 28);
	std::ofstream os4("sum.csv", std::ios_base::out);
	if (os4.is_open()) {
		to_csv(os4, sr4);
		os4.close();
	}
	auto sr5 = run_experiment(sum_round_robin, 1u, 1u << 28);
	std::ofstream os5("sum_round_robin.csv", std::ios_base::out);
	if (os5.is_open()) {
		to_csv(os5, sr5);
		os5.close();
	}
	auto sr6 = run_experiment(round_robin, 1u, 1u << 28);
	std::ofstream os6("round_robin.csv", std::ios_base::out);
	if (os6.is_open()) {
		to_csv(os6, sr6);
		os6.close();
	}
	auto sr7 = run_experiment(mutex_rb_rr, 1u, 1u << 28);
	std::ofstream os7("mutex_rb_rr.csv", std::ios_base::out);
	if (os7.is_open()) {
		to_csv(os7, sr7);
		os7.close();
	}

	auto sr8 = run_experiment(sum_round_robin_No_Localization, 1u, 1u << 28);
	std::ofstream os8("sum_round_robin_No_Localization.csv", std::ios_base::out);
	if (os8.is_open()) {
		to_csv(os8, sr8);
		os8.close();
	}
	//std::cout << myPow::pow(Z<~0u>(5u), 13u).get() << "\n";
	/*const int sz = 100;
	unsigned* v = new unsigned[sz];
	for (int i = 0; i < sz; i++) {
		v[i] = 0;
	}
	randomize(v, sz, 10, 20, 0);
	for (int i = 0; i < sz; i++) {
		std::cout << v[i] << " ";
	}*/
	return 0;
}

