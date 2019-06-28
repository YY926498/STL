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
	template<typename T, typename Alloc = alloc>
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
		using iterator = _list_iterator<value_type, reference, pointer>;
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
		//释放一个节点
		void put_node(link_type p)
		{
			list_node_allocator::deallocate(p);
		}
		//产生（配置并构造）一个节点，带有元素值
		link_type create_node(const T& x)
		{
			link_type p = get_node();
			construct(&p->data, x);
			return p;
		}
		//销毁（析构并释放）一个节点
		void destory_node(link_type p)
		{
			destory(&p->data);
			put_node(p);
		}
		void empty_initialize()
		{
			node = get_node();//配置一个节点空间，令node指向它
			node->next = node;//令node头尾都指向自己，不设元素值
			node->prev = node;
		}
		//将[first,last)内的所有元素移动到position之前
		void transfer(iterator position, iterator first, iterator last)
		{
			if (position != last)
			{
				last.node->prev->next = position.node;
				first.node->prev->next = last.node;
				position.node->prev->next = first.node;
				link_type tmp = position.node->prev;
				position.node->prev = last.node->prev;
				last.node->prev = first.node->prev;
				first.node->prev = tmp;
			}
		}
	public:
		//构造函数
		list() { empty_initialize(); }

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
		//函数目的：在迭代器position所指位置插入一个节点，内容化为x
		iterator insert(iterator position, const T& x)
		{
			link_type tmp = create_node(x);//产生一个节点（设内容为x）
			//调整双向指针，是tmp也插入进去
			tmp->next = position.node;
			tmp->prev = position.node->prev;
			position.node->prev->next = tmp;
			position.node->prev = tmp;
			return tmp;

		}
		//插入一个节点，作为尾节点
		void push_back(const T& x)
		{
			insert(end(), x);
		}
		//插入一个节点，作为头结点
		void push_front(const T& x)
		{
			insert(begin(), x);
		}
		//移除迭代器position所指节点
		iterator erase(iterator position)
		{
			link_type next_node = position.node->next;
			link_type prev_node = position.node->prev;
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destory_node(position.node);
			return next_node;
		}
		//移除头结点
		void pop_front()
		{
			erase(begin());
		}
		//移除尾结点
		void pop_back()
		{
			iterator tmp = end();
			erase(--tmp);
		}
		//清除所有节点
		void clear();
		//将数值为value的所有元素移除
		void remove(const T&);
		//移除数值相同的连续元素。注意，只有”连续而相同的元素“，才会被移除剩一个
		void unique();
		//将x接合于position所指位置之前。x必须不同于*this
		void splice(iterator position, list& x);
	};
	template<typename T, typename Alloc>
	void list<T, Alloc>::clear()
	{
		link_type cur = node->next;//begin()
		while (cur != node)
		{
			link_type tmp = cur;
			cur = cur->next;
			destory_node(tmp);//销毁（析构并释放）一个节点
		}
		//恢复node原始状态
		node->next = node;
		node->prev = node;
	}
	template<typename T, typename Alloc>
	void list<T, Alloc>::remove(const T& value)
	{
		iterator first = begin();
		iterator last = end();
		while (first != last)
		{
			iterator next = first;
			++next;
			if (*first == value)
				erase(first);//找到就移除
			first = next;
		}
	}
	template<typename T, typename Alloc>
	void list<T, Alloc>::unique()
	{
		iterator first = begin();
		iterator last = end();
		if (first == last)
			return;//空链表，什么都不做
		iterator next = first;
		while (++next != last)
		{
			if (*first == *next)//遍历每个节点，如果有相同的元素，移除
				erase(next);
			else
				first = next;//调整指针
			next = first;//修正区段范围
		}
	}
	template<typename T, typename Alloc>
	void list<T, Alloc>::splice(iterator position, list& x)
	{
		if(&x!=this && )
	}
}


#endif
