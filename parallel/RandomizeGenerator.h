#pragma once
#include <iostream>
#include <concepts>
#include <type_traits>
#include <thread>
#include <vector>
#include "num_threads.h"

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

namespace myPow {
	template <class T> concept monoid = requires (T x) { T(); x *= x; };
	template <monoid T, std::unsigned_integral U>
	T pow(T x, U n) {
		//Реализация двоичного алгоритма возведения в степень
		auto r = T();
		while (n > 0) {
			if ((n & 1) != 0) {
				r *= x;
			}
			x *= x;
			n >>= 1;
		}
		return r;
	}
}

//Z_32 -> [x_min, x_max] 
//Элемент в сс 32 отображается в элемент от x_min до x_max
//template <class F> requires std::is_invocable_r <unsigned, F, unsigned> // Было
template <class F> requires std::invocable<F, unsigned>
void affine_transoform(unsigned aa, unsigned bb, F map, unsigned* v, size_t n, unsigned x0) {
	unsigned T = get_num_threads();
	std::vector <std::thread> w(T - 1);

	auto worker = [v, aa, bb, map, n, x0, T](unsigned t) {
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
			v[i] = map(myPow::pow(gen, i)(x0));
		}
	};

	for (unsigned t = 1; t < T; t++) {
		w.emplace_back(worker, t);
	}
	worker(0);
	for (auto& thr : w) {
		thr.join();
	}

	//Доделать это дома
	// Пример вызова ниже:
	/*Например,у нас есть вектор v размером n, и пусть у нас a = 3, b = 4
	Тогда вызов аффинного преобразования: affine_transoform(3, 4, [](auto x) {return x; }, v, n, 10)
	тогда наши рандомные значения должны быть: 10, 34, 106, 302, 970 и т.д.*/
}


//Генерация случайных чисел в вектор v размером n; числа от x_min до x_max; entropy выступает seed'ом
void randomize(unsigned* v, size_t n, unsigned x_min, unsigned x_max, unsigned entropy) {
	unsigned a = 3; // parameter of affine transformation
	unsigned b = 7; // same
	unsigned range = x_max - x_min + 1;

	// Lambda to transformate numbers into interval [x_min, x_max]
	auto map = [range, x_min](unsigned x) -> unsigned {
		return (x % range) + x_min;
	};

	/*
		entropy (seed) parameter using to set starting x value (x0) to get random numbers
		if a = 3, b = 4 and entropy = 10 then x0 = 10
		v[0] = 10
		v[1] = a * v[0] + b = 3 * 10 + 4 = 34
		v[2] = a * v[1] + b = 3 * 34 + 4 = 106
		...
		v[n] = a * v[n-1] + b
	*/

	// call affine transform
	affine_transoform(a, b, map, v, n, entropy);
}



