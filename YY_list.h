#ifndef _YY_LIST_H_
#define _YY_LIST_H_

namespace YY
{
	template<typename T>
	struct _list_node
	{
		using pointer=_list_node<T>*;
		pointer prev;
		pointer next;
		T data;
	};

	template<typename T, typename Ref, typename Ptr>
	struct _list_iterator
	{
		using iterator = _list_iterator<T, T&, T*>;
		using self = _list_iterator<T, Ref, Ptr>;
		using iterator_category = bidirectional_iterator_tag;//list iterator 为双向迭代器
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using link_type = _list_node<T>*;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		link_type node;//迭代器内部有一个普通指针，指向list的节点

		//constructor
		_list_iterator(link_type x) :node(x) {}
		_list_iterator() :node(nullptr) {}
		_list_iterator(const iterator& x) :node(x.node) {}

		bool operator==(const self& x)const { return node == x.node; }
		bool operator!=(const self& x)const { return node != x.node; }
		//以下对迭代器取值(dereference)，取得是节点的数据值
		reference operator*()const { return node->data; }

		//以下是迭代器的成员存取(member access)运算子的标准做法
		pointer operator->()const { return &(operator*()); }

		//对迭代器累加1，就是前进一个节点
		self& operator++()
		{
			node = node->next;
			return *this;
		}
		self operator++(int)
		{
			self tmp = *this;
			++(*this);
			return tmp;
		}

		//对迭代器减1，就是后退一个节点
		self& operator--()
		{
			node = node->prev;
			return *this;
		}
		self operator--(int)
		{
			self tmp = *this;
			--(*this);
			return tmp;
		}
	};

	//list不仅是一个双向链表，还是一个环形链表，所以只需要一个指针，便可以完整表现整个链表
	template<typename T,typename Alloc=alloc>
	class list
	{
	protected:
		using list_node = _list_node<T>;
		//专属空间配置器，每次配置一个节点大小
		using list_node_allocator = simple_alloc<list_node, Alloc>;
	public:
		using link_type = list_node *;
		using value_type = T;
		using pointer = value_type *;
		using reference = value_type &;
		using iterator = _list_iterator<value_type,reference,pointer>;		
		using size_type = size_t;
		using difference_type = ptrdiff_t;
	protected:
		//让指针node指向刻意置于尾后的一个空白节点，node便能符合STL对于"前闭后开"区间的要求
		link_type node;//只要一个指针，便可以表示整个环形链表
		
		//配置一个节点并传回
		link_type get_node()
		{
			return list_node_allocator::allocate();
		}

	public:
		iterator begin()
		{
			return node->next;
		}
		iterator end()
		{
			return node;
		}
		bool empty() const
		{
			return node->next == node;
		}
		size_type size() const
		{
			size_type result = 0;
			result = distance(begin(), end());
			return result;
		}
		//取头结点的内容(元素值)
		reference front()
		{
			return *begin();
		}
		//取尾结点的内容(元素值)
		reference back()
		{
			return *(--end());
		}
	};
}

#endif
