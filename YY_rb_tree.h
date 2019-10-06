#include "YY_iterator.h"
#include "YY_allocator.h"
#include "YY_algorithm.h"
#include <utility>
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
		static base_ptr maximum(base_ptr x)
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
				//����ڵ�һֱ�Ǹ��ڵ���Һ��ӣ�����Ҫ���ϻ��ݣ�ֱ���ҵ����������ֱ�Ӻ�̽ڵ�
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

	inline bidirectional_iterator_tag iterator_category(const _rb_tree_iterator_base&)
	{
		return bidirectional_iterator_tag{};
	}

	inline _rb_tree_iterator_base::difference_type* distance_type(const _rb_tree_iterator_base&)
	{
		return static_cast<_rb_tree_iterator_base::difference_type*>(0);
	}
	template<typename Value, typename Ref, typename Ptr>
	inline  Value* value_type(const _rb_tree_iterator < Value, Ref, Ptr>&)
	{
		return static_cast<Value*>(0);
	}


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

		//����������������������ȡ�ýڵ�x�ĳ�Ա
		static link_type& left(link_type x)
		{
			return static_cast<link_type&>(x->left);
		}
		static link_type& right(link_type x)
		{
			return static_cast<link_type&>(x->right);
		}
		static link_type& parent(link_type x)
		{
			return static_cast<link_type&>(x->parent);
		}
		static reference value(link_type x)
		{
			return x->value_field;
		}
		static const Key& key(link_type x)
		{
			return KeyOfValue()(value(x));
		}
		static color_type& color(link_type x)
		{
			return static_cast<color_type&>(x->color);
		}

		//��������������������ȡ�ýڵ�x�ĳ�Ա
		static link_type& left(base_ptr x)
		{
			return static_cast<link_type&>(x->left);
		}
		static link_type& right(base_ptr x)
		{
			return static_cast<link_type&>(x->right);
		}
		static link_type& parent(base_ptr x)
		{
			return static_cast<link_type&>(x->parent);
		}
		static reference value(base_ptr x)
		{
			return static_cast<link_type>(x)->value_field;
		}
		static const Key& key(base_ptr x)
		{
			return KeyOfValue()(value(link_type(x)));
		}
		static color_type& color(base_ptr x)
		{
			return static_cast<color_type&>(link_type(x)->color);
		}

		//��ȡ����ֵ�ͼ�Сֵ��
		static link_type minimum(link_type x)
		{
			return static_cast<link_type>(_rb_tree_node_base::minimum(x));
		}
		static link_type maximum(link_type x)
		{
			return static_cast<link_type>(_rb_tree_node_base::maximum(x));
		}

	private:
		iterator _insert(base_ptr x, base_ptr y, const value_type& v);
		link_type _copy(link_type x, link_type p);
		void _erase(link_type x);
		void init()
		{
			header = get_node();//����һ���ڵ�ռ䣬��headerָ����
			color(header) = _rb_tree_red;//��headerΪ��ɫ����������header��root
			root() = nullptr;
			leftmost() = header;//��header��ڵ�Ϊ����
			rightmost() = header;//��header�ҽڵ�Ϊ����
		}
	public:
		rb_tree(const Compare& comp = Compare()) :node_count(0), key_compare(comp) { init(); }
		~rb_tree()
		{
			clear();
			put_node(header);
		}
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

	public:
		Compare key_comp()const { return key_compare; }
		iterator begin() { return leftmost(); }
		iterator end() { return header; }//RB�����յ�Ϊheader��ָ��
		bool empty() const { return node_count == 0; }
		size_type size() const { return node_count; }
		size_type max_size() const { return size_type(-1); }
	public:
		//��x���뵽RB-TREE�У����ֽڵ�ֵ��һ�޶�
		std::pair<iterator, bool> insert_unique(const value_type& x);
		//��x���뵽RB-TREE�У�����ڵ�ֵ�ظ���
		iterator insert_equal(const value_type& x);
	};

	//Ԫ�ز���������ڵ��ֵ�����ظ�������ֵ��һ��RB-TREE��������ָ�������ڵ�
	template<typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc>
	typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator 
		rb_tree< Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v)
	{
		link_type y = header;
		link_type x = root();//�Ӹ��ڵ㿪ʼ
		while (x != nullptr)
		{
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}
		//xΪ��ֵ�����,yΪ�����֮���ڵ㣬vΪ��ֵ
		return _insert(x, y, v);
	}
	//Ԫ�ز���������ڵ�ֵ�������ظ������ظ��������Ч
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,bool>
		rb_tree< Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v)
	{
		link_type y = header;
		link_type x = root();//�Ӹ��ڵ㿪ʼ
		bool comp = true;
		while (x != nullptr)
		{
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}
		//�뿪whileѭ��֮��y��ָ�������֮���ڵ�
		iterator j = iterator(y);
		if (comp)//����뿪whileѭ��ʱcompΪ�棨��ʾ�����󡱣������������
		{
			return std::pair<iterator, bool>(_insert(x, y, v), true);
		}
		else
			--j;//����j����ͷ׼������
		//�¼�ֵ������м�ֵ�ظ�������ִ�а������
		if (key_compare(key(j.node), KeyOfValue()(v)))
			return std::pair<iterator, bool>(_insert(x, y, v), true);
		//���ϣ�xΪ��ֵ����㣬yΪ�����֮���ڵ㣬vΪ��ֵ
		//�������ˣ���ʾ��ֵһ�������м�ֵ�ظ�����ô�Ͳ��ò�����ֵ
		return std::pair<iterator, bool>(j, false);
	}

	
	//�����Ĳ���ִ�г���
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
		typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_insert(base_ptr x_,base_ptr y_, const Value& v)
	{
		//����x_Ϊ��ֵ����㣬����y_Ϊ�����֮���ڵ㣬����vΪ��ֵ
		link_type x = static_cast<link_type>(x_);
		link_type y = static_cast<link_type>(y_);
		link_type z;
		//key_compare�Ǽ�ֵ��С�Ƚ�׼��
		if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y)))
		{
			z = create_node(v);//����һ���½ڵ�
			left(y) = z;
			if (y == header)
			{
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost())//���yΪ����ڵ�
			{
				leftmost() = z;
			}
		}
		else
		{
			z = create_node(v);
			right(y) = z;
			if (y == rightmost())
				rightmost() = z;
		}
		parent(z) = y;
		left(z) = nullptr;
		right(z) = nullptr;
		_rb_tree_rebalance(z, header->parent);//����һΪ�����ڵ㣬������Ϊroot
		++node_count;
		return iterator(z);//����һ����������ָ�������ڵ�
	}

	//��������
	inline void _rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root)
	{
		//xΪ��ת��
		_rb_tree_node_base* y = x->right;
		x->right = y->left;
		if (y->left != nullptr)
			y->left->parent = x;
		y->parent = x->parent;
		//��y��ȫ����x�ĵ�λ
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}
	//��������
	inline void _rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root)
	{
		//xΪ��ת��
		_rb_tree_node_base* y = x->left;//yΪ��תʱ�����ӽڵ�
		x->left = y->right;
		if (y->right != nullptr)
			y->right->parent = x;
		y->parent = x->parent;
		//��y��ȫ����x�ĵ�λ
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->right = x;
		x->parent = y;
	}


	//����������,����������ƽ��(�ı���ɫ����ת����)
	inline void _rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base*& root)
	{
		x->color = _rb_tree_red;//�½ڵ��Ϊ��
		while (x != root && x->parent->color == _rb_tree_red)//���ڵ�Ϊ��
		{
			if (x->parent == x->parent->parent->left)//���ڵ�Ϊ�游�ڵ�����ӽڵ�
			{
				_rb_tree_node_base* y = x->parent->parent->right;//��yΪ�����ڵ�
				if (y && y->color == _rb_tree_red)//�����ڵ���ڣ���Ϊ��
				{
					x->parent->color = _rb_tree_black;
					y->color = _rb_tree_black;//�����ڵ㼰�����ڵ���ɫ��Ϊ��
					x->parent->parent->color = _rb_tree_red;
					x = x->parent->parent;
				}
				else//�޲����ڵ���߲����ڵ�Ϊ��ɫ
				{
					if (x == x->parent->right)//����½ڵ�Ϊ���ڵ�����ӽڵ�
					{
						x = x->parent;
						_rb_tree_rotate_left(x, root);//��һ����Ϊ������
					}
					x->parent->color = _rb_tree_black;//�ı���ɫ
					x->parent->parent->color = _rb_tree_red;
					_rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else
			{
				_rb_tree_node_base* y = x->parent->parent->left;//��yΪ�����ڵ�
				if (y && y->color == _rb_tree_red)//�в����ڵ㣬��Ϊ��
				{
					x->parent->color = _rb_tree_black;
					y->color = _rb_tree_black;
					x->parent->parent->color = _rb_tree_red;
					x = x->parent->parent;
				}
				else
				{
					if (x == x->parent->left)//����½ڵ�Ϊ���ڵ�����ӽڵ�
					{
						x = x->parent;
						_rb_tree_rotate_right(x, root);
					}
					x->parent->color = _rb_tree_black;
					x->parent->parent->color = _rb_tree_red;
					_rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}
		root->color = _rb_tree_black;//���ڵ���ԶΪ��
	}
	//ɾ��һ���ڵ�����
	inline _rb_tree_node_base* _rb_tree_rebanlance_for_erase(
		_rb_tree_node_base* z, _rb_tree_node_base*& root,
		_rb_tree_node_base*& leftmost, _rb_tree_node_base*& rightmost
	)
	{
		_rb_tree_node_base* y = z;
		_rb_tree_node_base* x = nullptr;
		_rb_tree_node_base* x_parent = nullptr;
		if (y->left == nullptr)//z������һ���պ���
			x = y->right;	//x����Ϊ��
		else
			if (y->right == nullptr)//z����һ���ǿսڵ�
				x = y->left;//xһ���ǿ�
			else//z�����Һ��Ӿ��ǿ�
			{
				y = y->right;
				while (y->left != nullptr)
					y = y->left;
				x = y->right;//x����Ϊ��
			}
		if (y != z)//��ʱyΪz��ֱ�Ӻ��
		{
			z->left->parent = y;
			y->left = z->left;
			if (y != z->right)
			{
				x_parent = y->parent;
				if (x)
					x->parent = y->parent;
				y->parent->left = x;//yһ�����丸�׵�����
				y->right = z->right;
				z->right->parent = y;
			}
			else
			{
				x_parent = y;
			}
			if (root == z)
				root = y;
			else if (z->parent->left == z)
				z->parent->left = y;
			else
				z->parent->right = y;
			swap(y->color, z->color);
			y = z;
		}
		else//y==z
		{
			x_parent = y->parent;
			if (x)
				x->parent = y->parent;
			if (root == z)
				root = x;
			else if (z->parent->left == z)
				z->parent->left = x;
			else
				z->parent->right = x;

			if (leftmost == z)
			{
				if (z->right == nullptr)//z�����ӱ�Ϊnull
					leftmost = z->parent;
				else
					leftmost = _rb_tree_node_base::minimum(x);
			}
			if (rightmost == z)
			{
				if (z->left == nullptr)//z���Һ��ӱ�Ϊnull
					rightmost = z->parent;
				else
					rightmost = _rb_tree_node_base::maximum(x);
			}
		}
		if (y->color == _rb_tree_red)
		{
			while (x != root && (x == nullptr || x->color == _rb_tree_black))
			{
				if (x == x_parent->left)
				{
					_rb_tree_node_base* w = x_parent->right;
					if (w->color == _rb_tree_red)
					{
						w->color = _rb_tree_black;
						x_parent->color = _rb_tree_red;
						_rb_tree_rotate_left(x_parent, root);
						w = x_parent->right;
					}
					if ((w->left == nullptr || w->left->color == _rb_tree_black) &&
						(w->right == nullptr || w->right->color == _rb_tree_black))
					{
						w->color = _rb_tree_red;
						x = x_parent;
						x_parent = x_parent->parent;
					}
					else
					{
						if (w->right == nullptr || w->right->color == _rb_tree_black)
						{
							if (w->left)
								w->left->color = _rb_tree_black;
							w->color = _rb_tree_red;
							_rb_tree_rotate_right(w, root);
							w = x_parent->right;
						}
						w->color = x_parent->color;
						x_parent->color = _rb_tree_black;
						if (w->right)
							w->right->color = _rb_tree_black;
						_rb_tree_rotate_left(x_parent, root);
						break;
					}
				}
				else
				{
					_rb_tree_node_base* w = x_parent->left;
					if (w->color == _rb_tree_red)
					{
						w->color = _rb_tree_black;
						x->parent->color = _rb_tree_red;
						_rb_tree_rotate_right(x_parent, root);
						w = x_parent->left;
					}
					if ((w->right == nullptr || w->right->color == _rb_tree_black) &&
						(w->left == nullptr || w->left->color == _rb_tree_black))
					{
						w->color = _rb_tree_red;
						x = x_parent;
						x_parent = x_parent->parent;
					}
					else
					{

					}
				}
			}
		}
	}

}

#endif
