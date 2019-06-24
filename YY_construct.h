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
	//以下是destroy()的第一版本，接受一个指针
	template<typename T>
	inline void destory(T* pointer)
	{
		pointer->~T();//调用析构函数
	}

	//如果元素的数值型别(value type)有non-trivial destructor...
	template<typename ForwardIterator>
	inline void  _destory_aux(ForwardIterator first, ForwardIterator last, _false_type)
	{
		for (; first < last; ++first)
			destory(&*first);
	}
	
	//如果元素的数值型别(value type)有trivial destructor...
	template<typename ForwardIterator>
	inline void  _destory_aux(ForwardIterator first, ForwardIterator last, _true_type)
	{
		/*for (; first < last; ++first)
			destory(&*first);*/
	}

	//判断元素的数值型别(value type)是否有trivial destructor
	template<typename ForwardIterator,typename T>
	inline void _destory(ForwardIterator first, ForwardIterator last, T*)
	{
		using trivial_destructor = typename _type_traits<T>::has_trivial_destructor;
		_destory_aux(first, last, trivial_destructor());
	}

	//以下是destory第二版本，接受两个迭代器，此函数设法找出元素的数值型别
	//进而利用_type_traits<>求取最适当措施
	template<typename ForwardIterator>
	inline void destory(ForwardIterator first, ForwardIterator last)
	{
		_destory(first, last, value_type(first));
	}
}


#endif

