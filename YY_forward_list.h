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

	//单向链表的节点结构
	template<typename T>
	struct _forward_list_node :public _forward_list_node_base
	{
		T data;
	};

	//全局函数：已知某一节点，插入新节点于其后
	inline _forward_list_node_base* _forward_list_make_list(_forward_list_node_base* prev_node,
		_forward_list_node_base* new_node)
	{
		//令new节点的下一节点为prev节点的下一节点
		new_node->next = prev_node->next;
		prev_node->next = new_node;	//令prev节点的下一节点指向new节点
		return new_node;
	}
	//全局函数：单向链表的大小（元素个数）
	inline size_t _forward_list_size(_forward_list_node_base* node)
	{
		size_t result = 0;
		for (; node != nullptr; node = node->next)
			++result;
		return result;
	}

	//单向链表的迭代器基本结构
	struct _forward_list_iterator_base
	{
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator_category = forward_iterator_tag;

		_forward_list_node_base* node;//指向节点基本结构

		_forward_list_iterator_base(_forward_list_node_base* x) :node(x) {}

		void incr() { node = node->next; }//前进一个节点

		bool operator==(const _forward_list_iterator_base& x)const
		{
			return node == x.node;
		}
		bool operator!=(const _forward_list_iterator_base& x)const
		{
			return node != x.node;
		}
	};

	//单向链表的迭代器结构
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
		//调用forward_list<T>::end()时会造成_forward_list_iterator(0),于是调用上述函数
		_forward_list_iterator() :_forward_list_iterator_base(nullptr) {}
		_forward_list_iterator(const iterator& x) :_forward_list_iterator_base(x.node) {}

		reference operator*() const { return (static_cast<list_node*>(node))->data; }
		pointer operator->()const { return &(operator*()); }
	};
	
}


#endif

