#include <cstddef>
#include <iterator>
#include "YY_iterator.h"
#ifndef _YY_FORWARD_LIST_
#define _YY_FORWARD_LIST_

namespace YY
{
	struct _forward_list_node_base
	{
		_forward_list_node_base* next;
	};

	//��������Ľڵ�ṹ
	template<typename T>
	struct _forward_list_node :public _forward_list_node_base
	{
		T data;
	};

	//ȫ�ֺ�������֪ĳһ�ڵ㣬�����½ڵ������
	inline _forward_list_node_base* _forward_list_make_list(_forward_list_node_base* prev_node,
		_forward_list_node_base* new_node)
	{
		//��new�ڵ����һ�ڵ�Ϊprev�ڵ����һ�ڵ�
		new_node->next = prev_node->next;
		prev_node->next = new_node;	//��prev�ڵ����һ�ڵ�ָ��new�ڵ�
		return new_node;
	}
	//ȫ�ֺ�������������Ĵ�С��Ԫ�ظ�����
	inline size_t _forward_list_size(_forward_list_node_base* node)
	{
		size_t result = 0;
		for (; node != nullptr; node = node->next)
			++result;
		return result;
	}

	//��������ĵ����������ṹ
	struct _forward_list_iterator_base
	{
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator_category = forward_iterator_tag;

		_forward_list_node_base* node;//ָ��ڵ�����ṹ

		_forward_list_iterator_base(_forward_list_node_base* x) :node(x) {}

		void incr() { node = node->next; }//ǰ��һ���ڵ�

		bool operator==(const _forward_list_iterator_base& x)const
		{
			return node == x.node;
		}
		bool operator!=(const _forward_list_iterator_base& x)const
		{
			return node != x.node;
		}
	};

	//��������ĵ������ṹ
	template<typename T,typename Ref, typename Ptr>
	struct _forward_list_iterator :public _forward_list_iterator_base
	{
		using iterator = _forward_list_iterator<T, T&, T*>;
		using const_iterator = _forward_list_iterator<T, const T&, const T*>;
		using self = _forward_list_iterator = _forward_list_iterator<T, Ref, Ptr>;

		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using list_node = _forward_list_node<T>;

		_forward_list_iterator(list_node* x) :_forward_list_iterator_base(x) {}
		//����forward_list<T>::end()ʱ�����_forward_list_iterator(0),���ǵ�����������
		_forward_list_iterator() :_forward_list_iterator_base(nullptr) {}
		_forward_list_iterator(const iterator& x) :_forward_list_iterator_base(x.node) {}

		reference operator*() const { return (static_cast<list_node*>(node))->data; }
		pointer operator->()const { return &(operator*()); }
	};
	
}


#endif

