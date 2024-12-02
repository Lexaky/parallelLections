#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include "num_threads.h"
#include <fstream>
#include "own_barrier_latch.h"
#include <thread>
#include "ExperimentsRunner.cpp"
#include "Sums.h"


// Method to create csv file from vector that contains scalability_result objects
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
	unsigned* vect = new unsigned[1000000];
	for (unsigned i = 0, j = 0; i < 1000000; i++, j++) {
		vect[j] = i;
	}
	std::cout << localization_sum(vect, 1000000);


	return 0;
}

