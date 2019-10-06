#ifndef _YY_ALGORITHM_H_
#define _YY_ALGORITHM_H_
#include "YY_iterator.h"
#include "YY_functional.h"
#include <corecrt_memcpy_s.h>
#include "YY_type_traits.h"
namespace YY
{
	//�����������distance����
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

	//�����������advance����
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
		//��n����ѭ���Ĵ������ٶȿ�
		for (Distance n = last - first; n > 0; --n, ++result, ++first)
		{
			*result = *first;
		}
		return result;
	}
	//���°汾�����ڡ�ָ����ָ����߱�trivial assigment operator��
	template<typename T>
	inline T* _copy_t(const T* first,const T* last,T* result,_true_type)
	{
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}
	//���°汾�����ڡ�ָ����ָ����߱�non-trivial assignment operator��
	template<typename T>
	inline T* _copy_t(const T* first, const T* last, T* result, _false_type)
	{
		//ԭ��ָ����һ��RandomAccessIterator�����Խ���_copy_d()���
		return _copy_d(first, last, result, (ptrdiff_t*)0);
	}
	//�ػ�InputIterator
	template<typename InputIterator,typename OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
	{
		//���������ͬ��񣬾���ѭ���Ƿ�������ٶ���
		for (; first != last; ++first, ++result)
		{
			*result = *first;
		}
		return result;
	}
	//�ػ�RandomAccessIterator
	template<typename RandomAccessIterator,typename OutputIterator>
	inline OutputIterator _copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag)
	{
		//�ֻ��ֳ�һ��������Ϊ���������ط�Ҳ�����õ�
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

	//������ʽ1
	inline char* copy(const char* first, const char* last, char* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}
	//������ʽ2
	inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
	{
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}
	//copy��ȫ�����汾
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

	template<typename T>
	inline void swap(T& lhs, T& rhs)
	{
		T temp = lhs;
		lhs = rhs;
		rhs = temp;
	}


	//���㷨
	//������push_heap��ʵ��ϸ��
	//��������push_back()������ָ��"��С�Ƚϱ�׼"
	template<typename RandomAccessIterator,typename Distance,typename T,typename Compare>
	void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value,Compare comp)
	{
		Distance parent = (holeIndex - 1) / 2;//�ҳ����ڵ�
		while (holeIndex > topIndex && comp(value , *(first + parent)))
		{
			//��δ���ﶥ�ˣ��Ҹ��ڵ�С����ֵ
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}//���������ˣ�������heap��������Ϊֹ
		*(first + holeIndex) = value;//��ɲ������
	}
	template<typename RandomAccessIterator,typename Distance, typename T,typename Compare>
	inline void _push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*,Compare comp)
	{
		_push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)),comp);
	}
	template<typename RandomAccessIterator,typename Compare = less<typename iterator_traits<RandomAccessIterator>::value_type>>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last,Compare comp= Compare{})
	{
		//ע�⣬�˺���������ʱ����Ԫ��Ӧ�����ڵײ���������β��
		_push_heap_aux(first, last, distance_type(first), value_type(first),comp);
	}
	//������pop_heap��ʵ��ϸ��
	template<typename RandomAccessIterator,typename Distance,typename T,typename Compare>
	void _adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp)
	{
		Distance secondChild = 2 * holeIndex + 2;//���ڵ���ҽڵ�
		while (secondChild < len && (!comp(value, *(first + secondChild)) || !comp(value, *(first + (secondChild - 1)))))
		{
			//�Ƚ϶��ڵ�֮��������ֵ��Ȼ����secondChild����ϴ�ڵ�
			if (comp(*(first + secondChild-1) , *(first + (secondChild))))
				secondChild--;
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			secondChild = 2 * holeIndex + 2;
		}
		*(first + holeIndex) = value;
		if (secondChild == len && !comp(value, *(first + (secondChild - 1))))
		{
			secondChild--;
			*(first + holeIndex) = *(first + secondChild);
			*(first + secondChild) = value;
		}
	}
	//��������pop_heap������ָ��"��С�Ƚϱ�׼"
	template<typename RandomAccessIterator,typename T,typename Distance,typename Compare>
	inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last,
						  RandomAccessIterator result, T value, Distance*,Compare comp)
	{
		*result = *first;//�趨βֵΪ��ֵ������βֵ��Ϊ������
		_adjust_heap(first, Distance(0), Distance(last - first), value,comp);
		//�������µ���heap������Ϊ0��������ֵΪvalue
	}
	template<typename RandomAccessIterator,typename T,typename Compare>
	inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*,Compare comp)
	{
		_pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first),comp);
	}
	template<typename RandomAccessIterator,typename Compare = less<typename iterator_traits<RandomAccessIterator>::value_type>>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last,Compare comp = Compare{})
	{
		_pop_heap_aux(first, last, value_type(first),comp);
	}
	//������sort_heap���㷨ϸ��
	template<typename RandomAccessIterator,typename Compare = less<typename iterator_traits<RandomAccessIterator>::value_type>>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp=Compare{})
	{
		//���£�ÿִ��һ��pop_heap����ֵ��������β��
		//�۳�β����ִ��һ��pop_heap���μ�ֵ�ֱ�������β�ˡ�һֱ��ȥ����󼴵õ�������
		while (last - first > 1)
			pop_heap(first, last--,comp);//ÿִ��pop_heapһ�Σ�������Χ��С1��
	}
	//������make_heap���㷨ϸ��
	//��������make_heap()������ָ��"��С�Ƚϱ�׼"
	template<typename RandomAccessIterator,typename T,typename Distance, typename Compare>
	void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*, Compare comp)
	{
		if (last - first < 2)
			return;//�������С��2��������������
		Distance len = last - first;
		//�ҳ���һ����Ҫ���ŵ�����ͷ���������κ�Ҷ�ڵ㶼����ִ������
		Distance holeIndex = (len - 2) / 2;
		while (true)
		{
			//������holeIndexΪ�׵�����
			_adjust_heap(first, holeIndex, len, T(*(first + holeIndex)),comp);
			if (holeIndex == 0)
				return;
			--holeIndex;
		}
	}
	template<typename RandomAccessIterator,typename Compare = less<typename iterator_traits<RandomAccessIterator>::value_type>>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp = Compare{})
	{
		_make_heap(first, last, value_type(first), distance_type(first),comp);
	}
}

#endif

