#include <cstddef>
#include <iterator>
#include "YY_iterator.h"
#include "YY_allocator.h"
#ifndef _YY_FORWARD_LIST_
#define _YY_FORWARD_LIST_

namespace YY
{
	struct _forward_list_node_base
	{
		_forward_list_node_base* next;
		_forward_list_node_base(_forward_list_node_base* x=nullptr) :next(x) {}
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
		using self = _forward_list_iterator<T, Ref, Ptr>;

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

		self& operator++()
		{
			incr();//前进一个节点
			return *this;
		}
		self operator++(int)
		{
			self tmp = *this;
			incr();
			return tmp;
		}
	};
	
	template<typename T,typename Alloc = alloc>
	class forward_list
	{
	public:
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = const value_type*;
		using reference = value_type &;
		using const_reference = const value_type &;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using iterator = _forward_list_iterator<T, T&, T*>;
		using const_iterator = _forward_list_iterator<T, const T&, const T*>;

	private:
		using list_node = _forward_list_node<T>;
		using list_node_base = _forward_list_node_base;
		using iterator_base = _forward_list_iterator_base;
		using list_node_allocator = simple_alloc<list_node, Alloc>;

		static list_node* create_node(const value_type& x)
		{
			list_node* node = list_node_allocator::allocate();//配置空间
			construct(&node->data, x);
			node->next = nullptr;
			return node;
		}
		static void destory_node(list_node* node)
		{
			destory(&node->data);//将元素析构
			list_node_allocator::deallocate(node);//释放空间
		}

	private:
		list_node_base head;//头部，不是指针，是实体

	public:
		explicit forward_list(): head(){  }
		explicit forward_list(const std::initializer_list<T>& x) 
		{
			auto it = x.end();
			--it;
			auto beg = x.begin();
			for (; it != beg; --it)
				push_front(*it);
			push_front(*beg);
		}
		~forward_list() { clear(); }

	public:
		iterator begin() { return iterator(static_cast<list_node*>(head.next)); }
		iterator end() { return iterator(nullptr); }
		size_type size()const { return _forward_list_size(head.next); }
		bool empty()const { return head.next == nullptr; }

		//两个forward_list互换，只要将head交换互换即可
		void swap(forward_list& L)
		{
			list_node_base* tmp = head.next;
			head.next = L.head.next;
			L.head.next = tmp;
		}

	public:
		//取头部元素
		reference front() { return static_cast<list_node*>(head.next)->data; }
		//从头部插入元素（新元素成为forward_list的第一个元素
		void push_front(const value_type& x)
		{
			_forward_list_make_list(&head, create_node(x));
		}

		//注意，没有push_back
		//从头部取走元素（删除），修改head
		void pop_front()
		{
			if (head.next == nullptr)
				return;
			list_node* node = static_cast<list_node*>(head.next);
			head.next = node->next;
			destory_node(node);
		}

		void clear()
		{
			while (head.next != nullptr)
			{
				list_node* tmp = static_cast<list_node*>(head.next);
				head.next = tmp->next;
				destory_node(tmp);
			}
		}
	};
}


#endif

