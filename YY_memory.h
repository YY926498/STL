#ifndef _YY_MEMORY_H_
#define _YY_MEMORY_H_

#include "YY_iterator.h"
#include "YY_type_traits.h"
#include "YY_algorithm.h"
namespace YY
{
	//以下就是否为POD型别，采取最适当的措施
	//如果不是POD型别，就会派送到这里
	template<typename ForwardIterator,typename Size,typename T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type)
	{
		ForwardIterator cur = first;
		for (; n > 0; --n, ++cur)
		{
			construct(&*cur, x);
		}
		return cur;
	}
	//如果是POD型别，就会被派送到这里
	//如果copy construction 等同于assigment，而且有trivial destructor
	//以下就有效
	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type)
	{
		return fill_n(first,n,x);
	}
	//首先以value_type()萃取出迭代器first的value type，再利用_type_traits判断型别是否为POD型别
	template<typename ForwardIterator, typename Size, typename T,typename T1>
	inline ForwardIterator _uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
	{
		using is_POD=typename _type_traits<T1>::is_POD_type;
		return _uninitialized_fill_n_aux(first, n, x, is_POD());
	}
	template<typename ForwardIterator,typename Size,typename T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
	{
		return _uninitialized_fill_n(first, n, x, value_type(first));
	}
	template<typename ForwardIterator,typename T>
	inline ForwardIterator uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value)
	{
		return uninitialized_fill_n(first, distance(first, last), value);
	}
	//以下就是否为POD型别，采取最适当的措施
	//如果不是POD型别，就会派送到这里
	template<typename ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(ForwardIterator first, ForwardIterator last, ForwardIterator dest, _false_type)
	{
		while (first != last)
		{
			construct(&*dest, *first);
			++dest;
		}
		return dest;
	}
	//如果是POD型别，就会被派送到这里
	//如果copy construction 等同于assigment，而且有trivial destructor
	//以下就有效
	template<typename ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(ForwardIterator first, ForwardIterator last, ForwardIterator dest, _true_type)
	{
		typename iterator_traits<ForwardIterator>::difference_type size = distance(first, last);
		using value_type=typename iterator_traits<ForwardIterator>::value_type;
		memcpy_s(dest,size*sizeof(value_type) , first, size*sizeof(value_type));
		advance(dest, size);
		return dest;
	}
	//首先以value_type()萃取出迭代器first的value type,再利用_type_traits判断型别是否为POD型别
	template<typename ForwardIterator,typename T>
	inline ForwardIterator _uninitialized_copy(ForwardIterator first, ForwardIterator last, ForwardIterator dest, T*)
	{
		using is_POD=typename _type_traits<T>::is_POD_type;
		return _uninitialized_copy_aux(first, last, dest, is_POD());
	}
	template<typename ForwardIterator>
	inline ForwardIterator uninitialized_copy(ForwardIterator first, ForwardIterator last, ForwardIterator dest)
	{
		return _uninitialized_copy(first, last, dest, value_type(first));
	}
}

#endif


