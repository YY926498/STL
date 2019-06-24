#ifndef _YY_CONSTRUCT_H_
#define _YY_CONSTRUCT_H_

#include "YY_type_traits.h"
#include "YY_iterator.h"
#include <new>

namespace YY
{
	template<typename T1,typename T2>
	inline void construct(T1* p, const T2& value)
	{
		new(p) T1(value);//placement new
	}
	//������destroy()�ĵ�һ�汾������һ��ָ��
	template<typename T>
	inline void destory(T* pointer)
	{
		pointer->~T();//������������
	}

	//���Ԫ�ص���ֵ�ͱ�(value type)��non-trivial destructor...
	template<typename ForwardIterator>
	inline void  _destory_aux(ForwardIterator first, ForwardIterator last, _false_type)
	{
		for (; first < last; ++first)
			destory(&*first);
	}
	
	//���Ԫ�ص���ֵ�ͱ�(value type)��trivial destructor...
	template<typename ForwardIterator>
	inline void  _destory_aux(ForwardIterator first, ForwardIterator last, _true_type)
	{
		/*for (; first < last; ++first)
			destory(&*first);*/
	}

	//�ж�Ԫ�ص���ֵ�ͱ�(value type)�Ƿ���trivial destructor
	template<typename ForwardIterator,typename T>
	inline void _destory(ForwardIterator first, ForwardIterator last, T*)
	{
		using trivial_destructor = typename _type_traits<T>::has_trivial_destructor;
		_destory_aux(first, last, trivial_destructor());
	}

	//������destory�ڶ��汾�������������������˺����跨�ҳ�Ԫ�ص���ֵ�ͱ�
	//��������_type_traits<>��ȡ���ʵ���ʩ
	template<typename ForwardIterator>
	inline void destory(ForwardIterator first, ForwardIterator last)
	{
		_destory(first, last, value_type(first));
	}
}


#endif

