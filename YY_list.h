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
	template<typename T,typename Alloc=alloc>
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
		using iterator = _list_iterator<value_type,reference,pointer>;		
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
	};
}

#endif
