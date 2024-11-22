#include "own_barrier_latch.h"

void Latch::arrive_and_wait() {
	std::unique_lock l(this->mtx);
	if (--T == 0) { 
		// ≈сли после изменени€ количества потоков они равны 0,
		// тогда дождались выполнени€ всех потоков, работа "люка" завершена
		cv.notify_all();
	}
	else {
		do
			cv.wait(l);
		while (T > 0);
	}
}

void Barrier::arrive_and_wait() {
	std::unique_lock l(this->mtx);
	if (--T == 0) {
		T = T0; // ¬осстановили параметр T
		this->barrierGeneration = !this->barrierGeneration; // —казали о том, что следующее поколение барьера может быть создано (а текущее не актуально)
		cv.notify_all();
	}
	else {
		bool myBarrier = this->barrierGeneration;
		while (myBarrier == this->barrierGeneration)
			cv.wait(l);
	}
}