#include <iostream>
#include "own_barrier_latch.h"
#include "num_threads.h"
// �������� - ��� ����������� �������� ����� ��������� � �������: S^n -> S^m, m < n
// ������������ �������� - ��� ����� �� ������ ��� ����������� (��� � ���������� �������
// o - �������� ������� k >= 2
// e - ����������� ������� (����������� ��� ��������)
// ��������������� o

void part_sum() {
	size_t m = get_num_threads();
	for (size_t neighbor = 1, next = 2; neighbor < m; neighbor = next, next += next) {
		if (t % next == 0 && t + neighbor < m) {
			partitial_sums[t] += partitial_sums[t + neighbor];
		}
	// ���-�� ����� ���������� ������������ ������. �������������� ��������, �� ���-�� ����� ��� ���������, ���� � if
	// ���� ��� ���-��
	}
	return paritital_sum[0];
}