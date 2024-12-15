#pragma once
#include <iostream>
#include <concepts>
#include <type_traits>

class af {
private:
	unsigned a, b;
public:
	af() {
		a = 1;
		b = 0;
	}
	af(unsigned aa, unsigned bb) : a(aa), b(bb) {}

	unsigned operator ()(unsigned x) const {
		return a * x + b;
	}

	af& operator *=(af& x) {
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

//��������� ��������� ����� � ������ v �������� n; ����� �� x_min �� x_max; entropy ��������� seed'��
double randomize(unsigned *v, size_t n, unsigned x_min, unsigned x_max, unsigned entropy) {
	/*
		����� ����� ���������� ������� affine_transoform(unisgned a, unsigned b)
	*/
}
//Z_32 ->[x_min, x_max] 
//������� � �� 32 ������������ � ������� �� x_min �� x_max
template <class F> requires std::is_invocable_r <unsigned, F, unsigned>
void affine_transoform(unsigned aa, unsigned bb, F map, const unsigned *v, size_t n, unsigned x0) {
	unsigned T = get_num_threads();
	std::vector <std::thread> v(T);
	auto worker = [v, aa, ab, map, v, n, x0](unsigned t) {
		unsigned s, b, e;
		s = n / T;
		b = n % T;
		if (t < b)
			b = 1 + s * t;
		else
			b += s * t;
		e = b + s;
		af gen(aa, bb);
		for (size_t i = b; i < e; i++) {
			v[i] = map(pow(gen, i)(x0));
		}
	}


		//�������� ��� ����
		// ������ ������ ����:
		��������,� ��� ���� ������ v �������� n, � ����� � ��� a = 3, b = 4
		����� ����� ��������� ��������������: affine_transoform(3, 4, [](auto x) {return x; }, v, n, 10)
		����� ���� �������� �������� ������ ����: 10, 34, 106, 302, 970 � �.�.
}

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