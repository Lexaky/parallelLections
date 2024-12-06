#pragma once
#include <iostream>
#include <concepts>
#include <type_traits>

class Monoid {
private:
	unsigned a, b;
public:
	Monoid() {
		a = 1;
		b = 0;
	}
	Monoid& operator *=(Monoid& x) {
		a = a * x.a;
		b = a * x.b + b;
		return *this;
	}
};

template <unsigned n>
class Z {
private:
	unsigned x;
public:
	Z() {
		x = 1;
	}
	Z(unsigned v):x(v) {}

	Z& operator *= (const Z& el) {
		x = (x * el.x)%n;
		return *this;
	}
	unsigned get() const {
		return x;
	}
};

////��������� ��������� ����� � ������ v �������� n; ����� �� x_min �� x_max; entropy ��������� seed'��
//double randomize(unsigned *v, size_t n, unsigned x_min, unsigned x_max, unsigned entropy) {
//	/*
//		����� ����� ���������� ������� affine_transoform(unisgned a, unsigned b)
//	*/
//}
////Z_32 ->[x_min, x_max] 
////������� � �� 32 ������������ � ������� �� x_min �� x_max
//template <class F> requires std::is_invocable_r <unsigned, F, unsigned>
//void affine_transoform(unsigned a, unsigned b, F map) {
//
//}

namespace myPow {
	template <class T> concept monoid = requires (T x) { T(); x *= x; };
	template <monoid T, std::unsigned_integral U>
	T pow(T x, U n) {
		//���������� ��������� ��������� ���������� � �������
		auto r = T();
		while (n > 0) {
			if (n & 1 != 0) {
				r *= x;
			}
			x *= x;
			n >>= 1;
		}
		return r;
	}

}