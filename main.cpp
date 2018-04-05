#include <iostream>
using namespace std;

template<class...Args>
struct type_list;
template<>
struct type_list<>
{
};
template<class T>
struct type_list<T>
{
	using first = T;
	using remain = type_list<>;
};
template<class T, class...Args>
struct type_list<T, Args...>
{
	using first = T;
	using remain = type_list<Args...>;
};

template<class T>
struct type_list_first;
template<class T, class...Args>
struct type_list_first<type_list<T, Args...>>
{
	using type = T;
};

template<class T>
struct type_list_last;
template<class T>
struct type_list_last<type_list<T>>
{
	using type = T;
};
template<class T, class...Args>
struct type_list_last<type_list<T, Args...>>
{
	using type = typename type_list_last<type_list<Args...>>::type;
};

template<class T, class...Args>
struct type_list_push_front;
template<class...Args1, class...Args2>
struct type_list_push_front<type_list<Args1...>, Args2...>
{
	using type = type_list<Args2..., Args1...>;
};

template<class T>
struct type_list_pop_front;
template<class T, class...Args>
struct type_list_pop_front<type_list<T, Args...>>
{
	using type = type_list<Args...>;
};

template<class T, class...Args>
struct type_list_push_back;
template<class...Args1, class...Args2>
struct type_list_push_back<type_list<Args1...>, Args2...>
{
	using type = type_list<Args1..., Args2...>;
};

template<class...Args>
struct type_list_cat;
template<class...Args1, class...Args2>
struct type_list_cat<type_list<Args1...>, type_list<Args2...>>
{
	using type = type_list<Args1..., Args2...>;
};
template<class...Args1, class...Args2, class...Tuples>
struct type_list_cat<type_list<Args1...>, type_list<Args2...>, Tuples...>
{
	using type = typename type_list_cat< type_list<Args1..., Args2...>, Tuples...>::type;
};

template<class T>
struct type_list_reverse;
template<>
struct type_list_reverse<type_list<>>
{
	using type = type_list<>;
};
template<class T>
struct type_list_reverse<type_list<T>>
{
	using type = type_list<T>;
};
template<class T, class...Args>
struct type_list_reverse<type_list<T, Args...>>
{
	using type = typename type_list_push_back< typename type_list_reverse<typename type_list<Args...>>::type, T>::type;
};

template<class T>
struct type_list_pop_back;
template<class...Args>
struct type_list_pop_back<type_list<Args...>>
{
	using type = typename type_list_reverse< typename type_list_pop_front< typename type_list_reverse< type_list<Args...> >::type >::type >::type;
};

template<class Tup, int n, class T>
struct type_list_insert;
template<class...Args, int n, class T, class First>
struct type_list_insert<type_list<First, Args...>, n, T>
{
	template<unsigned n>
	struct process
	{
		using type = typename type_list_push_front< typename type_list_insert< type_list<Args...>, n - 1, T >::type, First >::type;
	};
	template<>
	struct process<0>
	{
		using type = type_list<T, First, Args...>;
	};

	using type = typename process<n>::type;
};

template<class Tup, int n>
struct type_list_get;
template<class T, class...Args>
struct type_list_get<type_list<T, Args...>, 0>
{
	using type = T;
};
template<class T, class...Args, int n>
struct type_list_get<type_list<T, Args...>, n>
{
	using type = typename type_list_get<type_list<Args...>, n - 1>::type;
};

template<class Tup, class T>
struct type_list_find;
template<class T>
struct type_list_find<type_list<>, T>
{
	static const size_t value;
};
template<class T, class...Args, class First>
struct type_list_find<type_list<First, Args...>, T>
{
private:
	template<class T1, class T2>
	struct process
	{
		static const size_t value = type_list_find<type_list<Args...>, T>::value + 1;
	};
	template<class T1>
	struct process<T1, T1>
	{
		static const size_t value = 0;
	};
public:
	static const size_t value = process<First, T>::value;
};

template<class Tup, size_t n>
struct type_list_right;
template<size_t n, class First, class...Args>
struct type_list_right<type_list<First, Args...>, n>
{
private:
	template<size_t i>
	struct process
	{
		using type = typename type_list_right<type_list<Args...>, i - 1>::type;
	};
	template<>
	struct process<0>
	{
		using type = type_list<Args...>;
	};
public:
	using type = typename process<n>::type;
};

template<class Tup, size_t n>
struct type_list_left;
template<size_t n, class...Args>
struct type_list_left<type_list<Args...>, n>
{
private:
	template<size_t i>
	struct process
	{
		using temp = typename process<i - 1>::type;

		using type = typename type_list_push_front<
			temp,
			typename type_list_get< type_list<Args...>, i >::type
		>::type;
	};
	template<>
	struct process<0>
	{
		using type = type_list<typename type_list_first< type_list<Args...> >::type>;
	};
public:
	using type = typename process<n-1>::type;
};

template<class Tup, size_t n>
struct type_list_erase;
template<class...Args, size_t n>
struct type_list_erase<type_list<Args...>, n>
{
	using type = typename type_list_cat<
		typename type_list_left<type_list<Args...>, n>::type,
		typename type_list_right<type_list<Args...>, n>::type
	>::type;
};

template<class Tup, size_t n, class T>
struct type_list_assign;
template<class...Args, size_t n, class T>
struct type_list_assign<type_list<Args...>, n, T>
{
	using type = typename type_list_cat<
		typename type_list_left<type_list<Args...>, n>::type,
		type_list<T>,
		typename type_list_right<type_list<Args...>, n>::type
	>::type;
};

template<class T>
struct type_list_size;
template<>
struct type_list_size<type_list<>>
{
	static const size_t value = 0;
};
template<class T, class...Args>
struct type_list_size<type_list<T, Args...>>
{
	static const size_t value = type_list_size<Args...> +1;
};

template<class T, template<class Elem1, class Elem2> class CompareFunc>
struct type_list_sort;
template<template<class Elem1, class Elem2> class CompareFunc>
struct type_list_sort<type_list<>, CompareFunc>
{
	using type = type_list<>;
};
template<class T, template<class Elem1, class Elem2> class CompareFunc>
struct type_list_sort<type_list<T>, CompareFunc>
{
	using type = type_list<T>;
};
template<class T1, class T2, template<class Elem1, class Elem2> class CompareFunc>
struct type_list_sort<type_list<T1, T2>, CompareFunc>
{
private:
	template<bool less>
	struct process
	{
		using type = type_list<T1, T2>;
	};
	template<>
	struct process<false>
	{
		using type = type_list<T2, T1>;
	};
public:
	using type = typename process< CompareFunc<T1, T2>::value >::type;
};
template<class T1, class T2, class...Args, template<class Elem1, class Elem2> class CompareFunc>
struct type_list_sort<type_list<T1, T2, Args...>, CompareFunc>
{
private:
	using first_pair = typename type_list_sort< type_list<T1, T2>, CompareFunc >::type;
	using type_list_first_pair_sorted = typename type_list_push_front<
		typename type_list_sort< type_list<typename first_pair::remain::first, Args...>, CompareFunc >::type,
		typename type_list_first<first_pair>::type
	>::type;
public:
	using type = typename type_list_push_back <
		typename type_list_sort < typename type_list_pop_back<type_list_first_pair_sorted>::type, CompareFunc> ::type,
		typename type_list_last<type_list_first_pair_sorted>::type
	>::type;
};

template<class T1, class T2>
struct compare_sizeof
{
private:
	template<class T>
	struct r_size
	{
		static const size_t value = sizeof(T);
	};
	template<>
	struct r_size<void>
	{
		static const size_t value = 0;
	};
	template<class T>
	struct r_size<T &>
	{
		static const size_t value = 0;
	};
	template<class T>
	struct r_size<T &&>
	{
		static const size_t value = 0;
	};
	template<class FuncRet, class...FuncArgs>
	struct r_size<FuncRet(FuncArgs...)>
	{
		static const size_t value = 0;
	};
public:
	static const bool value = r_size<T1>::value < r_size<T2>::value;
};
template<class T>
struct type_list_sort_sizeof : public type_list_sort<T, compare_sizeof> {};

template<class T1, class T2>
struct compare_value
{
	static const bool value = T1::value < T1::value;
};

int main()
{
	typedef type_list<double, int, char, short, char[233], short[3], int&, void> Tup;
	typedef type_list_sort_sizeof<Tup>::type Tup2;
	cout << typeid(Tup).name() << endl;
	cout << typeid(Tup2).name() << endl;
}
