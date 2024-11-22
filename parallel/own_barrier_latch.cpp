#include "own_barrier_latch.h"

void Latch::arrive_and_wait() {
	std::unique_lock l(this->mtx);
	if (--T == 0) { 
		// ���� ����� ��������� ���������� ������� ��� ����� 0,
		// ����� ��������� ���������� ���� �������, ������ "����" ���������
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
		T = T0; // ������������ �������� T
		this->barrierGeneration = !this->barrierGeneration; // ������� � ���, ��� ��������� ��������� ������� ����� ���� ������� (� ������� �� ���������)
		cv.notify_all();
	}
	else {
		bool myBarrier = this->barrierGeneration;
		while (myBarrier == this->barrierGeneration)
			cv.wait(l);
	}
}