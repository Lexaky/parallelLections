#pragma once
#include <iostream>
#include <concepts>
#include <type_traits>

//√енераци€ случайных чисел в вектор v размером n; числа от x_min до x_max; entropy выступает seed'ом
double randomize(unsigned *v, size_t n, unsigned x_min, unsigned x_max, unsigned entropy) {
	/*
		«десь будет вызыватьс€ функци€ affine_transoform(unisgned a, unsigned b)
	*/
}
//Z_32 ->[x_min, x_max] 
//Ёлемент в сс 32 отображаетс€ в элемент от x_min до x_max
template <class F> requires std::is_invocable_r <unsigned, F, unsigned>
void affine_transoform(unsigned a, unsigned b, F map) {

}

template <class T> concept monoid = requires (T x) { T(); x *= x; };
template <monoid T, std::unsigned_integral U>
T pow(T x, U n) {
	//–еализаци€ двоичного алгоритма возведени€ в степень
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

