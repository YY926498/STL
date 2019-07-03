#ifndef _YY_MEMORY_H_
#define _YY_MEMORY_H_

#include "YY_iterator.h"
#include "YY_type_traits.h"
#include "YY_algorithm.h"
namespace YY
{
	//���¾��Ƿ�ΪPOD�ͱ𣬲�ȡ���ʵ��Ĵ�ʩ
	//�������POD�ͱ𣬾ͻ����͵�����
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
	//�����POD�ͱ𣬾ͻᱻ���͵�����
	//���copy construction ��ͬ��assigment��������trivial destructor
	//���¾���Ч
	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type)
	{
		return fill_n(first,n,x);
	}
	//������value_type()��ȡ��������first��value type��������_type_traits�ж��ͱ��Ƿ�ΪPOD�ͱ�
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
	//���¾��Ƿ�ΪPOD�ͱ𣬲�ȡ���ʵ��Ĵ�ʩ
	//�������POD�ͱ𣬾ͻ����͵�����
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
	//�����POD�ͱ𣬾ͻᱻ���͵�����
	//���copy construction ��ͬ��assigment��������trivial destructor
	//���¾���Ч
	template<typename ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(ForwardIterator first, ForwardIterator last, ForwardIterator dest, _true_type)
	{
		typename iterator_traits<ForwardIterator>::difference_type size = distance(first, last);
		using value_type=typename iterator_traits<ForwardIterator>::value_type;
		memcpy_s(dest,size*sizeof(value_type) , first, size*sizeof(value_type));
		advance(dest, size);
		return dest;
	}
	//������value_type()��ȡ��������first��value type,������_type_traits�ж��ͱ��Ƿ�ΪPOD�ͱ�
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


