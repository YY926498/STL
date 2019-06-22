#ifndef _YY_ITERATOR_H_
#define _YY_ITERATOR_H_
#include <cstddef>
namespace YY
{
	//iterator tag types
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag :public input_iterator_tag {};
	struct bidirectional_iterator_tag :public forward_iterator_tag {};
	struct random_access_iterator_tag :public bidirectional_iterator_tag {};

	template<typename Category,
		typename T,
		typename Distance = ptrdiff_t,
		typename Pointer = T *,
		typename Reference = T &>
		struct iterator
	{
		using iterator_category=Category;
		using value_type=T;
		using difference_type=Distance;
		using pointer=Pointer;
		using reference=Reference;
	};

	//"ե֭��" traits
	template<typename Iterator>
	struct iterator_traits
	{
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
	};

	//���ԭ��ָ�����Ƶ�traitsƫ�ػ���
	template<typename T>
	struct iterator_traits<T*>
	{
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T *;
		using reference = T &;
	};

	//���ԭ��֮pointer to const����Ƶ�traitsƫ�ػ���
	template<typename T>
	struct iterator_traits<const T*>
	{
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = const T*;
		using reference = const T &;
	};

	//����������Ժܷ���ؾ���ĳ��������������(category)
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&)
	{
		using category = typename iterator_traits<Iterator>::iterator_category;
		return category();
	}

	//����������Ժܷ���ؾ���ĳ����������distance type
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	//���������������ĳ����������value_type
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	
}


#endif



