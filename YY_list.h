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
		using iterator_category = bidirectional_iterator_tag;//list iterator Ϊ˫�������
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using link_type = _list_node<T>*;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		link_type node;//�������ڲ���һ����ָͨ�룬ָ��list�Ľڵ�

		//constructor
		_list_iterator(link_type x) :node(x) {}
		_list_iterator() :node(nullptr) {}
		_list_iterator(const iterator& x) :node(x.node) {}

		bool operator==(const self& x)const { return node == x.node; }
		bool operator!=(const self& x)const { return node != x.node; }
		//���¶Ե�����ȡֵ(dereference)��ȡ���ǽڵ������ֵ
		reference operator*()const { return node->data; }

		//�����ǵ������ĳ�Ա��ȡ(member access)�����ӵı�׼����
		pointer operator->()const { return &(operator*()); }

		//�Ե������ۼ�1������ǰ��һ���ڵ�
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

		//�Ե�������1�����Ǻ���һ���ڵ�
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

	//list������һ��˫����������һ��������������ֻ��Ҫһ��ָ�룬���������������������
	template<typename T, typename Alloc = alloc>
	class list
	{
	protected:
		using list_node = _list_node<T>;
		//ר���ռ���������ÿ������һ���ڵ��С
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
		//��ָ��nodeָ���������β���һ���հ׽ڵ㣬node���ܷ���STL����"ǰ�պ�"�����Ҫ��
		link_type node;//ֻҪһ��ָ�룬����Ա�ʾ������������

		//����һ���ڵ㲢����
		link_type get_node()
		{
			return list_node_allocator::allocate();
		}
		//�ͷ�һ���ڵ�
		void put_node(link_type p)
		{
			list_node_allocator::deallocate(p);
		}
		//���������ò����죩һ���ڵ㣬����Ԫ��ֵ
		link_type create_node(const T& x)
		{
			link_type p = get_node();
			construct(&p->data, x);
			return p;
		}
		//���٣��������ͷţ�һ���ڵ�
		void destory_node(link_type p)
		{
			destory(&p->data);
			put_node(p);
		}
		void empty_initialize()
		{
			node = get_node();//����һ���ڵ�ռ䣬��nodeָ����
			node->next = node;//��nodeͷβ��ָ���Լ�������Ԫ��ֵ
			node->prev = node;
		}
		//��[first,last)�ڵ�����Ԫ���ƶ���position֮ǰ
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
		//���캯��
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
		//ȡͷ��������(Ԫ��ֵ)
		reference front()
		{
			return *begin();
		}
		//ȡβ��������(Ԫ��ֵ)
		reference back()
		{
			return *(--end());
		}
		//����Ŀ�ģ��ڵ�����position��ָλ�ò���һ���ڵ㣬���ݻ�Ϊx
		iterator insert(iterator position, const T& x)
		{
			link_type tmp = create_node(x);//����һ���ڵ㣨������Ϊx��
			//����˫��ָ�룬��tmpҲ�����ȥ
			tmp->next = position.node;
			tmp->prev = position.node->prev;
			position.node->prev->next = tmp;
			position.node->prev = tmp;
			return tmp;

		}
		//����һ���ڵ㣬��Ϊβ�ڵ�
		void push_back(const T& x)
		{
			insert(end(), x);
		}
		//����һ���ڵ㣬��Ϊͷ���
		void push_front(const T& x)
		{
			insert(begin(), x);
		}
		//�Ƴ�������position��ָ�ڵ�
		iterator erase(iterator position)
		{
			link_type next_node = position.node->next;
			link_type prev_node = position.node->prev;
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destory_node(position.node);
			return next_node;
		}
		//�Ƴ�ͷ���
		void pop_front()
		{
			erase(begin());
		}
		//�Ƴ�β���
		void pop_back()
		{
			iterator tmp = end();
			erase(--tmp);
		}
		//������нڵ�
		void clear();
		//����ֵΪvalue������Ԫ���Ƴ�
		void remove(const T&);
		//�Ƴ���ֵ��ͬ������Ԫ�ء�ע�⣬ֻ�С���������ͬ��Ԫ�ء����Żᱻ�Ƴ�ʣһ��
		void unique();
		//��x�Ӻ���position��ָλ��֮ǰ��x���벻ͬ��*this
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
			destory_node(tmp);//���٣��������ͷţ�һ���ڵ�
		}
		//�ָ�nodeԭʼ״̬
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
				erase(first);//�ҵ����Ƴ�
			first = next;
		}
	}
	template<typename T, typename Alloc>
	void list<T, Alloc>::unique()
	{
		iterator first = begin();
		iterator last = end();
		if (first == last)
			return;//������ʲô������
		iterator next = first;
		while (++next != last)
		{
			if (*first == *next)//����ÿ���ڵ㣬�������ͬ��Ԫ�أ��Ƴ�
				erase(next);
			else
				first = next;//����ָ��
			next = first;//�������η�Χ
		}
	}
	template<typename T, typename Alloc>
	void list<T, Alloc>::splice(iterator position, list& x)
	{
		if(&x!=this && )
	}
}


#endif
