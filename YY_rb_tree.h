#include "YY_iterator.h"
#include "YY_allocator.h"
#ifndef _YY_RB_TREE_H_
#define _YY_RB_TREE_H_

namespace YY
{
	using _rb_tree_color_type = bool;
	const _rb_tree_color_type _rb_tree_red = false;//��ɫΪ0
	const _rb_tree_color_type _rb_tree_black = true;//��ɫΪ1

	struct _rb_tree_node_base
	{
		using color_type = _rb_tree_color_type;
		using base_ptr = _rb_tree_node_base *;

		color_type color;//�ڵ���ɫ���Ǻڼ���
		base_ptr parent;//RB����������������֪�����ڵ�
		base_ptr left;
		base_ptr right;

		_rb_tree_node_base(color_type color_ = _rb_tree_red) :color(color_), parent(nullptr),
			left(nullptr), right(nullptr) {}

		static base_ptr minimum(base_ptr x)
		{
			while (x->left != nullptr)
				x = x->left;
			return x;
		}
		static base_ptr maxinum(base_ptr x)
		{
			while (x->right != nullptr)
				x = x->right;
			return x;
		}
	};

	template<typename Value>
	struct _rb_tree_node :public _rb_tree_node_base
	{
		using link_type = _rb_tree_node<Value>*;
		Value value_field;//�ڵ�ֵ
		_rb_tree_node(Value value=Value()) :_rb_tree_node_base(), value_field(value) {}
	};

	//���������
	struct _rb_tree_iterator_base
	{
		using base_ptr = _rb_tree_node_base::base_ptr;
		using iterator_category = bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;
		
		base_ptr node;//����������֮�����һ�����ӹ�ϵ
		
		
		_rb_tree_iterator_base():node(nullptr){}
		_rb_tree_iterator_base(base_ptr x) :node(x) {}
		void increment()
		{
			if (node == nullptr)
				return;
			if (node->right != nullptr)//��������ӽڵ�
			{
				node = node->right;
				while (node->left != nullptr)
					node = node->left;
			}
			else
			{
				base_ptr y = node->parent;//�ҳ����ڵ�
				while (node == y->right)
				{
					node = y;
					y = y->parent;
				}
				if (node->right != y)
					node = y;
			}
		}
		void decrement()
		{
			//����Ǻ�ڵ㣬�Ҹ��ڵ�ĸ��ڵ�������������ӽڵ㼴Ϊ��
			if (node->color == _rb_tree_red && node->parent->parent == node)
				node = node->right;
			else if (node->left != nullptr)
			{
				base_ptr y = node->left;
				while (y->right != nullptr)
					y = y->right;
				node = y;
			}
			else
			{
				base_ptr y = node->parent;
				while (node == y->left)
				{
					node = y;
					y = y->parent;
				}
				node = y;
			}
		}
	};
	
	template<typename Value,typename Ref,typename Ptr>
	struct _rb_tree_iterator :public _rb_tree_iterator_base
	{
		using value_type = Value;
		using reference = Ref;
		using pointer = Ptr;

		using iterator = _rb_tree_iterator<Value, Value&, Value*>;
		using const_iterator = _rb_tree_iterator<Value, const Value&, const Value*>;
		using self= _rb_tree_iterator<Value, Ref, Ptr>;
		using link_type  = _rb_tree_node<Value>*;

		_rb_tree_iterator() {}
		_rb_tree_iterator(link_type x) :_rb_tree_iterator_base(x) {}
		_rb_tree_iterator(const iterator& it) :_rb_tree_iterator_base(it.node) {}

		reference operator*()const
		{
			return static_cast<link_type>(node)->value_field;
		}
		pointer operator->()const
		{
			return &(operator*());
		}

		self& operator++()
		{
			increment();
			return *this;
		}
		self operator++(int)
		{
			self tmp = *this;
			increment();
			return tmp;
		}
		self& operator--()
		{
			decrement();
			return *this;
		}
		self operator--(int)
		{
			self tmp = *this;
			decrement();
			return tmp;
		}
	};


	//�������Ǻ�����Ķ���
	template<typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc=alloc>
	class rb_tree
	{
		using void_pointer = void*;
		using base_ptr=_rb_tree_node_base *;
		using rb_tree_node = _rb_tree_node<Value>;
		using rb_tree_node_allocator = simple_alloc<rb_tree_node, Alloc>;
		using color_type = _rb_tree_color_type;

	public:
		using key_type = Key;
		using value_type = Value;
		using pointer = value_type *;
		using const_pointer = const value_type*;
		using reference = value_type &;
		using const_reference = const value_type &;
		using link_type = rb_tree_node *;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator = _rb_tree_iterator<value_type, reference, pointer>;
		using const_iterator = _rb_tree_iterator<value_type, const_reference, const_pointer>;

	protected:
		link_type get_node()
		{
			return rb_tree_node_allocator::allocate();
		}
		void put_node(link_type p)
		{
			rb_tree_node_allocator::deallocate(p);
		}
		link_type create_node(const value_type& x)
		{
			link_type tmp = get_node();//���ÿռ�
			construct(&tmp->value_field, x);//��������
			return tmp;
		}
		link_type clone_node(link_type x)//����һ���ڵ�
		{
			link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = x->left;
			tmp->right = x->right;
			return tmp;
		}
		void destory_node(link_type p)
		{
			destory(&p->value_field);//��������
			put_node(p);//�ͷ��ڴ�
		}

	protected:
		//RB_TREE
		size_type node_count;//׷�ټ�¼���Ĵ�С���ڵ�������
		link_type header;//����ʵ���ϵ�һ������
		Compare key_compare;//�ڵ��ļ�ֵ��С�Ƚ�׼��

		//��������������������ȡ��header�ĳ�Ա
		link_type& root()const
		{
			return static_cast<link_type&>(header->parent);
		}
		link_type& leftmost() const
		{
			return static_cast<link_type&>(header->left);
		}
		link_type& rightmost()const
		{
			return static_cast<link_type&>(header->right);
		}

		//

	};
}

#endif
