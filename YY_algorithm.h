#ifndef _YY_ALGORITHM_H_
#define _YY_ALGORITHM_H_
#include "YY_iterator.h"
#include <corecrt_memcpy_s.h>
namespace YY
{
	//以下是整组的distance函数
	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type _distance(InputIterator first, InputIterator last, input_iterator_tag)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last)
		{
			++first;
			++n;
		}
		return n;
	}
	template<typename RandomAccessIterator>
	inline typename iterator_traits<RandomAccessIterator>::difference_type _distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
	{
		return last - first;
	}
	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last)
	{
		using category=typename iterator_traits<InputIterator>::iterator_category;
		return _distance(first, last, category());
	}

	//以下是整组的advance函数
	template<typename InputIterator, typename Distance>
	inline void _advance(InputIterator& i, Distance n, input_iterator_tag)
	{
		while (n--)
			++i;
	}
	template<typename BidirectionalIterator, typename Distance>
	inline void _advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
	{
		if (n >= 0)
			while (n--)
				++i;
		else
			while (n++)
				--i;
	}
	template<typename RandomAccessIterator, typename Distance>
	inline void _advance(RandomAccessIterator & i, Distance n, random_access_iterator_tag)
	{
		i += n;
	}
	template<typename InputIterator, typename Distance>
	inline void advance(InputIterator & i, Distance n)
	{
		_advance(i, n, iterator_category(i));
	}

	template<typename OutputIterator, typename Size, typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value)
	{
		for (; n > 0; --n, ++first)
			* first = value;
		return first;
	}

	template<typename RandomAccessIterator,typename OutputIterator,typename Distance>
	inline OutputIterator _copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		//以n决定循环的次数。速度快
		for (Distance n = last - first; n > 0; --n, ++result, ++first)
		{
			*result = *first;
		}
		return result;
	}
	//以下版本适用于“指针所指对象具备trivial assigment operator”
	template<typename T>
	inline T* _copy_t(const T* first,const T* last,T* result,_true_type)
	{
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}
	//以下版本适用于“指针所指对象具备non-trivial assignment operator”
	template<typename T>
	inline T* _copy_t(const T* first, const T* last, T* result, _false_type)
	{
		//原生指针是一种RandomAccessIterator，所以交给_copy_d()完成
		return _copy_d(first, last, result, (ptrdiff_t*)0);
	}
	//特化InputIterator
	template<typename InputIterator,typename OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
	{
		//与迭代器等同与否，决定循环是否继续。速度慢
		for (; first != last; ++first, ++result)
		{
			*result = *first;
		}
		return result;
	}
	//特化RandomAccessIterator
	template<typename RandomAccessIterator,typename OutputIterator>
	inline OutputIterator _copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag)
	{
		//又划分出一个函数，为的是其他地方也可能用到
		return _copy_d(first, last, result, distance_type(first));
	}
	//_copy_dispatch
	template<typename T>	
	T* _copy_dispatch(T* first, T* last, T* result)
	{
		using t=typename _type_traits<T>::has_trivial_assignment_operator;
		return _copy_t(first, last, result, t{});
	}
	template<typename T>
	T* _copy_dispatch(const T* first, const T* last, T* result)
	{
		using t=typename _type_traits<T>::has_trivial_assignment_operator;
		return _copy_t(first, last, result, t{});
	}
	template<typename InputIterator, typename OutputIterator>
	OutputIterator _copy_dispatch(InputIterator first, InputIterator last, OutputIterator result)
	{
		return _copy(first, last, result, iterator_category(first));
	}

	//特殊形式1
	inline char* copy(const char* first, const char* last, char* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}
	//特殊形式2
	inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
	{
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}
	//copy完全泛化版本
	template<typename InputIterator,typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return _copy_dispatch(first, last, result);
	}

	template<typename InputIterator,typename OutputIterator>
	inline void copy_backward(InputIterator first1, InputIterator last1, OutputIterator last2)
	{
		while (last1 != first1)
		{
			*(--last2) = *(--last1);
		}
	}
	template<typename T>
	inline T max(T lhs, T rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}
	template<typename Iterator>
	inline void iter_swap(Iterator lhs, Iterator rhs)
	{
		using value_type = typename Iterator::value_type;
		value_type tmp = *lhs;
		*lhs = *rhs;
		*rhs = tmp;
	}
}

#endif

