#pragma once
#include <concepts>
void set_num_threads(unsigned T);
unsigned get_num_threads();

struct scalability_result {
	unsigned result;
	double s, e; // ��������� � �������������
	//s = t1/tT, ��� t1 - ����� ���������� ������ ����� �������
	//e = S/T
};
//������������� �� C++20 ��� #include <concept>
template <std::invocable <const unsigned*, size_t> Functor>
std::vector<scalability_result> run_experiment(Functor f, unsigned v_size_min, unsigned v_size_max);
/*
	1) ���������������� ������ std::vector <scalability_result>
	   ���������� �� v_min �� v_max(�� ������������) (�� �������) - �� ���������� v_max - v_min
	2) ��������� ������� f ��� v �������� v_max - v_min
	3) ���������� ���������� � �������������� ������ ���������� 
	f, t1-t0
	4) ������� �������������� ������
*/