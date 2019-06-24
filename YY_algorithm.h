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

	//拷贝一个数组，其元素型别拥有non-trivial copy constructors
	template<typename T>
	void copy(T* source, T* destination, int n, _false_type)
	{
		while (n-- > 0)
		{
			construct(destination++, *source++);
		}
	}
	//拷贝一个数组，其元素型别拥有trivial copy constructors
	template<typename T>
	void copy(T* source, T* destination, int n, _true_type)
	{
		memcpy_s(destination, n*sizeof(T), source, n * sizeof(T));
	}

	//拷贝一个数组，其元素为任意型别，视情况采用最有效率的拷贝手段
	template<typename T>
	inline void copy(T* source, T* destination, int n)
	{
		copy(source, destination,n, typename _type_traits<T>::has_trivial_copy_constructor());
	}

	template<typename ForwardIterator,typename OutputIterator>
	inline void copy(ForwardIterator source, ForwardIterator end, OutputIterator destination)
	{
		typename iterator_traits<ForwardIterator>::difference_type size = distance(source, end);
		using value_type=typename iterator_traits<ForwardIterator>::value_type;
		copy(source, destination, size, typename _type_traits<value_type>::has_trivial_copy_constructor());
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
}

#endif

