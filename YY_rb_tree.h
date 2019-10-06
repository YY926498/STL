#include "YY_iterator.h"
#include "YY_allocator.h"
#include "YY_algorithm.h"
#include <utility>
#ifndef _YY_RB_TREE_H_
#define _YY_RB_TREE_H_

namespace YY
{
	using _rb_tree_color_type = bool;
	const _rb_tree_color_type _rb_tree_red = false;//红色为0
	const _rb_tree_color_type _rb_tree_black = true;//黑色为1

	struct _rb_tree_node_base
	{
		using color_type = _rb_tree_color_type;
		using base_ptr = _rb_tree_node_base *;

		color_type color;//节点颜色，非黑即白
		base_ptr parent;//RB树的许多操作，必须知道父节点
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
		Value value_field;//节点值
		_rb_tree_node(Value value=Value()) :_rb_tree_node_base(), value_field(value) {}
	};

	//基层迭代器
	struct _rb_tree_iterator_base
	{
		using base_ptr = _rb_tree_node_base::base_ptr;
		using iterator_category = bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;
		
		base_ptr node;//用来与容器之间产生一个连接关系
		
		
		_rb_tree_iterator_base():node(nullptr){}
		_rb_tree_iterator_base(base_ptr x) :node(x) {}
		void increment()
		{
			if (node == nullptr)
				return;
			if (node->right != nullptr)//如果有右子节点
			{
				node = node->right;
				while (node->left != nullptr)
					node = node->left;
			}
			else
			{
				base_ptr y = node->parent;//找出父节点
				//如果节点一直是父节点的右孩子，就需要不断回溯，直到找到中序遍历的直接后继节点
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
			//如果是红节点，且父节点的父节点等于自身，则右子节点即为答案
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


	//接下来是红黑树的定义
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
			link_type tmp = get_node();//配置空间
			construct(&tmp->value_field, x);//构造内容
			return tmp;
		}
		link_type clone_node(link_type x)//复制一个节点
		{
			link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = x->left;
			tmp->right = x->right;
			return tmp;
		}
		void destory_node(link_type p)
		{
			destory(&p->value_field);//析构内容
			put_node(p);//释放内存
		}

	protected:
		//RB_TREE
		size_type node_count;//追踪记录树的大小（节点数量）
		link_type header;//这是实现上的一个技巧
		Compare key_compare;//节点间的键值大小比较准则

		//以下三个函数用来方便取得header的成员
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

		//以下六个函数来用来方便取得节点x的成员
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

		//以下六个函数用来方便取得节点x的成员
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

		//求取极大值和极小值。
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
			header = get_node();//产生一个节点空间，令header指向它
			color(header) = _rb_tree_red;//令header为红色，用来区分header和root
			root() = nullptr;
			leftmost() = header;//令header左节点为自身
			rightmost() = header;//令header右节点为自身
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
		iterator end() { return header; }//RB树的终点为header所指处
		bool empty() const { return node_count == 0; }
		size_type size() const { return node_count; }
		size_type max_size() const { return size_type(-1); }
	public:
		//将x插入到RB-TREE中，保持节点值独一无二
		std::pair<iterator, bool> insert_unique(const value_type& x);
		//将x插入到RB-TREE中（允许节点值重复）
		iterator insert_equal(const value_type& x);
	};

	//元素插入操作，节点键值允许重复，返回值是一个RB-TREE迭代器，指向新增节点
	template<typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc>
	typename rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::iterator 
		rb_tree< Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v)
	{
		link_type y = header;
		link_type x = root();//从根节点开始
		while (x != nullptr)
		{
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}
		//x为新值插入点,y为插入点之父节点，v为新值
		return _insert(x, y, v);
	}
	//元素插入操作，节点值不允许重复，若重复则插入无效
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,bool>
		rb_tree< Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v)
	{
		link_type y = header;
		link_type x = root();//从根节点开始
		bool comp = true;
		while (x != nullptr)
		{
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}
		//离开while循环之后，y所指即插入点之父节点
		iterator j = iterator(y);
		if (comp)//如果离开while循环时comp为真（表示遇“大”，将插入于左侧
		{
			return std::pair<iterator, bool>(_insert(x, y, v), true);
		}
		else
			--j;//调整j，回头准备测试
		//新键值不与既有键值重复，于是执行安插操作
		if (key_compare(key(j.node), KeyOfValue()(v)))
			return std::pair<iterator, bool>(_insert(x, y, v), true);
		//以上，x为新值插入点，y为插入点之父节点，v为新值
		//进行至此，表示新值一定与树中键值重复，那么就不该插入新值
		return std::pair<iterator, bool>(j, false);
	}

	
	//真正的插入执行程序
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
		typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_insert(base_ptr x_,base_ptr y_, const Value& v)
	{
		//参数x_为新值插入点，参数y_为插入点之父节点，参数v为新值
		link_type x = static_cast<link_type>(x_);
		link_type y = static_cast<link_type>(y_);
		link_type z;
		//key_compare是键值大小比较准则
		if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y)))
		{
			z = create_node(v);//产生一个新节点
			left(y) = z;
			if (y == header)
			{
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost())//如果y为最左节点
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
		_rb_tree_rebalance(z, header->parent);//参数一为新增节点，参数二为root
		++node_count;
		return iterator(z);//返回一个迭代器，指向新增节点
	}

	//左旋操作
	inline void _rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root)
	{
		//x为旋转点
		_rb_tree_node_base* y = x->right;
		x->right = y->left;
		if (y->left != nullptr)
			y->left->parent = x;
		y->parent = x->parent;
		//令y完全顶替x的地位
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}
	//右旋操作
	inline void _rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root)
	{
		//x为旋转点
		_rb_tree_node_base* y = x->left;//y为旋转时的左子节点
		x->left = y->right;
		if (y->right != nullptr)
			y->right->parent = x;
		y->parent = x->parent;
		//令y完全顶替x的地位
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->right = x;
		x->parent = y;
	}


	//负责插入操作,重新令树形平衡(改变颜色及旋转树形)
	inline void _rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base*& root)
	{
		x->color = _rb_tree_red;//新节点必为红
		while (x != root && x->parent->color == _rb_tree_red)//父节点为红
		{
			if (x->parent == x->parent->parent->left)//父节点为祖父节点的左子节点
			{
				_rb_tree_node_base* y = x->parent->parent->right;//令y为伯父节点
				if (y && y->color == _rb_tree_red)//伯父节点存在，且为红
				{
					x->parent->color = _rb_tree_black;
					y->color = _rb_tree_black;//将父节点及伯父节点颜色变为黑
					x->parent->parent->color = _rb_tree_red;
					x = x->parent->parent;
				}
				else//无伯父节点或者伯父节点为黑色
				{
					if (x == x->parent->right)//如果新节点为父节点的右子节点
					{
						x = x->parent;
						_rb_tree_rotate_left(x, root);//第一参数为左旋点
					}
					x->parent->color = _rb_tree_black;//改变颜色
					x->parent->parent->color = _rb_tree_red;
					_rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else
			{
				_rb_tree_node_base* y = x->parent->parent->left;//令y为伯父节点
				if (y && y->color == _rb_tree_red)//有伯父节点，且为红
				{
					x->parent->color = _rb_tree_black;
					y->color = _rb_tree_black;
					x->parent->parent->color = _rb_tree_red;
					x = x->parent->parent;
				}
				else
				{
					if (x == x->parent->left)//如果新节点为父节点的左子节点
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
		root->color = _rb_tree_black;//根节点永远为黑
	}
	//删除一个节点后调整
	inline _rb_tree_node_base* _rb_tree_rebanlance_for_erase(
		_rb_tree_node_base* z, _rb_tree_node_base*& root,
		_rb_tree_node_base*& leftmost, _rb_tree_node_base*& rightmost
	)
	{
		_rb_tree_node_base* y = z;
		_rb_tree_node_base* x = nullptr;
		_rb_tree_node_base* x_parent = nullptr;
		if (y->left == nullptr)//z有至少一个空孩子
			x = y->right;	//x可能为空
		else
			if (y->right == nullptr)//z存在一个非空节点
				x = y->left;//x一定非空
			else//z的左右孩子均非空
			{
				y = y->right;
				while (y->left != nullptr)
					y = y->left;
				x = y->right;//x可能为空
			}
		if (y != z)//此时y为z的直接后继
		{
			z->left->parent = y;
			y->left = z->left;
			if (y != z->right)
			{
				x_parent = y->parent;
				if (x)
					x->parent = y->parent;
				y->parent->left = x;//y一定是其父亲的左孩子
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
				if (z->right == nullptr)//z的左孩子必为null
					leftmost = z->parent;
				else
					leftmost = _rb_tree_node_base::minimum(x);
			}
			if (rightmost == z)
			{
				if (z->left == nullptr)//z的右孩子必为null
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
