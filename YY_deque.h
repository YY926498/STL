#include "YY_allocator.h"
#include "YY_algorithm.h"
#include "YY_construct.h"
#include "YY_memory.h"
#ifndef _YY_DEQUE_H_
#define _YY_DEQUE_H_

namespace YY
{
	//如果n不为0，传回n，表示buffer size由用户自定义
	//如果n为0.表示buffer size使用默认值，那么
	//如果sz(元素大小，sizeof(value_type)小于512，传回512/sz
	//如果sz不小于512，传回1
	inline size_t _deque_buf_size(size_t n, size_t sz)
	{
		return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
	}
	template<typename T,typename Ref,typename Ptr,size_t BufSiz>
	struct _deque_iterator
	{//未继承iterator
		using iterator = _deque_iterator<T, T&, T*, BufSiz>;
		using const_iterator = _deque_iterator<T, const T&, const T*, BufSiz>;
		static size_t buffer_size()	{return _deque_buf_size(BufSiz, sizeof(T));	}
		//由于未继承iterator,所以必须自行撰写五个必要的迭代器相应型别
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using map_pointer = T * *;
		using self = _deque_iterator;

		//保持与容器的联结
		T* cur;//此迭代器所指之缓冲区的现行元素
		T* first;//此迭代器所指之缓冲区的头
		T* last;//此迭代器所指之缓冲区的尾（含备用空间）
		map_pointer node;//管控中心

		//用于跳一个缓冲区
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		//以下各个重载运算子是_deque_iterator<>成功运作的关键
		reference operator*()const
		{
			return *cur;
		}
		pointer operator->()const
		{
			return &(operator*());
		}
		difference_type operator-(const self& x) const
		{
			return difference_type(buffer_size())* (node - x.node - 1) + (cur - first) + (x.last - x.cur);
		}

		self& operator++()
		{
			++cur;//切换至下一元素
			if (cur == last)//如果已达所在缓冲区的尾端
			{
				set_node(node + 1);//就切换至下一个节点(亦即缓冲区)
				cur = first;//的第一个元素
			}
			return *this;
		}
		self operator++(int)
		{
			self tmp = *this;
			++ *this;
			return tmp;
		}
		self& operator--()
		{
			if (cur == first)
			{
				set_node(node - 1);
				cur = last;
			}
			--cur;
			return *this;
		}
		self operator--(int)
		{
			self tmp = *this;
			-- *this;
			return tmp;
		}
		//以下实现随机存取。迭代器可以直接跳跃n个距离
		self& operator+=(difference_type n)
		{
			difference_type offset = n + (cur - first);
			if (offset >= 0 && offset < difference_type(buffer_size()))
			{
				//目标在同一个缓冲区
				cur += n;
			}
			else
			{
				//目标不在同一个缓冲区
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size())
					: -difference_type((-offset - 1) / buffer_size()) - 1;
				//切换至正确的节点（亦即缓冲区）
				set_node(node + node_offset);
				//切换至正确的元素
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}
		//使用+=来替代单独的运算符
		self operator+(difference_type n)
		{
			self tmp = *this;
			return tmp += n;
		}
		self& operator-=(difference_type n)
		{
			return *this += -n;
		}
		self operator-(difference_type n)
		{
			self tmp = *this;
			return tmp -= n;
		}
		//以下实现随机存取，迭代器可以直接跳跃n个距离
		reference operator[](difference_type n)const
		{
			return *(*this + n);
		}
		bool operator==(const self& x)const
		{
			return cur == x.cur;
		}
		bool operator!=(const self& x)const
		{
			return !(*this == x);
		}
		bool operator<(const self& x)const
		{
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};

	template<typename T,typename Alloc=alloc,size_t BufSize=0>
	class deque
	{
	public:
		using value_type = T;
		using pointer = T *;
		using reference = T &;
		using const_reference = const reference;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator = _deque_iterator<T, T&, T*, BufSize>;
	protected:
		//元素指针的指针
		using map_pointer = pointer *;
		
		//数据成员
		iterator start;//指向第一个节点
		iterator finish;//指向最后一个节点
		map_pointer map;//指向map,map是一块连续空间，其每个元素都是个指针，指向一个节点（缓冲区）
		size_type map_size;//map内有多少指针

		//专属空间配置器，每次配置一个元素大小
		using data_allocator = simple_alloc<value_type, Alloc>;
		//专属空间配置器，每次配置一个指针大小
		using map_allocator = simple_alloc<pointer, Alloc>;

		void create_map_and_nodes(size_type num_elements);
		void fill_initialize(size_type n, const value_type& value);
		size_type initial_map_size()
		{
			return 8;
		}
		pointer allocate_node()
		{
			return (pointer)data_allocator::allocate(iterator::buffer_size());
		}
		void deallocate_node(pointer first)
		{
			data_allocator::deallocate(first, iterator::buffer_size());
		}
		void reserve_map_at_back(size_type nodes_to_add = 1)
		{
			//如果map尾端的节点备用空间不足
			//符合以上条件则必须重新换一个map(配置更大的，拷贝原来的，释放原来的)
			if (nodes_to_add + 1 > map_size - (finish.node - map))
				reallocate_map(nodes_to_add, false);
		}
		void reserve_map_at_front(size_type nodes_to_add = 1)
		{
			//如果map前端的节点备用空间不足
			//符合以上条件则必须重换一个map
			if (nodes_to_add > start.node - map)
				reallocate_map(nodes_to_add, true);
		}
		void push_back_aux(const value_type& t);
		void push_front_aux(const value_type& t);
		//只有finish.cur==finish.first才会调用
		void pop_back_aux();
		void pop_front_aux();
		void reallocate_map(size_type nodes_to_add, bool add_at_front);
		iterator insert_aux(iterator position, const value_type& value);
	public:
		deque() :start(), finish(), map(nullptr), map_size(0) {};
		deque(int n, const value_type& value = value_type{}) :start(), finish(), map(nullptr), map_size(0)
		{
			fill_initialize(n, value);
		}
		deque(const std::initializer_list<T>& lhs) :start(), finish(), map(nullptr), map_size(0)
		{
			create_map_and_nodes(lhs.size());//把deque的结构都产生并安排好
			auto beg = start;
			for (auto cur = lhs.begin(); cur != lhs.end(); ++cur,++beg)
			{
				*beg = *cur;
			}
		}
		void push_back(const value_type& t)
		{
			if (map == nullptr)
			{
				fill_initialize(1, t);
				return;
			}
			if (finish.cur != finish.last - 1)
			{
				//最后缓冲区至少有两个元素备用空间
				construct(finish.cur, t);
				++finish.cur;
			}
			else
				push_back_aux(t);//最后缓冲区只有一个备用空间
		}
		void push_front(const value_type& t)
		{
			if (start.cur != start.first)
			{
				construct(start.cur - 1, t);//第一缓冲区尚有可用空间，直接在备用空间上构造元素
				--start.cur;
			}
			else
				push_front_aux(t);//第一缓冲区已无备用空间
		}
		void pop_back()
		{
			if (finish.cur != finish.first)
			{
				//最后一个缓冲区有一个（或更多）元素
				--finish.cur;//调整指针，相当于排除了最后元素
				destory(finish.cur);
			}
			else
				pop_back_aux();//最后缓冲区没有任何元素
		}
		void pop_front()
		{
			if (start.cur != start.last - 1)
			{
				//第一缓冲区有两个及以上的元素
				destory(start.cur);//将第一个元素析构
				++start.cur;//调整指针，相当于排除了第一元素
			}
			else
				pop_front_aux();
		}
		iterator begin() { return start; }
		iterator end() { return finish; }
		reference operator[](size_type n)
		{
			return start[difference_type(n)];//调用_deque_iterator<>::operator[]
		}
		reference front()
		{
			return *start;
		}
		reference back()
		{
			//iterator tmp = finish;
			//--tmp;//调用_deque_iterator<>::operator--
			//return *tmp;
			return *(finish - 1);
		}
		size_type size()const
		{
			return finish - start;
		}
		size_type max_size()const
		{
			return size_type(-1);
		}
		bool empty()const
		{
			return finish == start;
		}
		//由于clear用于清除整个deque，但是deque的最初状态就是保由一个缓冲区，因此，clear后，要恢复初始状态
		void clear();
		//清除pos所指的元素。pos为清除点
		iterator erase(iterator pos)
		{
			iterator next = pos;
			++next;
			difference_type index = distance(start, pos);//清除点之前的元素个数
			if (index < (size() >> 1))//如果清除点之前的元素个数比较少
			{
				copy_backward(start, pos, next);//就移动清除点之前的元素
				pop_front();//移动完毕，最前一个元素冗余，去除之
			}
			else
			{
				copy(next, finish, pos);
				pop_back();
			}
			return start + index;
		}
		iterator erase(iterator first, iterator last);
		//在position处插入一个元素，其值为value
		iterator insert(iterator position, const value_type& value);
	};
	template<typename T,typename Alloc,size_t BufSize>
	void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value)
	{
		create_map_and_nodes(n);//把deque的结构都产生并安排好
		map_pointer cur;
		//为每个节点的缓冲区设定初值
		for (cur = start.node; cur < finish.node; ++cur)
		{			
			uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
		}
		//最后一个节点的设定稍有不同(因为尾端可能有备用空间，不必设初值)
		uninitialized_fill(finish.first, finish.cur, value);
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
	{
		//需要节点数=（元素个数/每个缓冲区可容纳的元素个数）+1
		//如果刚好整除，会多配一个节点
		size_type num_node = num_elements / iterator::buffer_size() + 1;

		//一个map要管理几个节点。最少8个，最多是所需节点数加2
		//前后各预留一个，扩充时可用
		map_size = max(initial_map_size(), num_node + 2);
		map = map_allocator::allocate(map_size);
		//以上配置出一个“具有map_size个节点”的map

		//以下令nstart和nfinish指向map所拥有之全部节点的最中央区域
		//保持在最中央，可使头尾两端的扩充能量一样大。每个节点可对应一个缓冲区
		map_pointer nstart = map + (map_size - num_node) / 2;
		map_pointer nfinish = nstart + num_node - 1;

		map_pointer cur;
		//为map内的每个现用节点配置缓冲区。所有缓冲区加起来就是deque的可用空间
		//（最后一个缓冲区可能留有一些余裕）
		//此处未考虑异常
		for (cur = nstart; cur <= nfinish; ++cur)
			* cur = allocate_node();
		start.set_node(nstart);
		finish.set_node(nfinish);
		start.cur = start.first;
		finish.cur = finish.first + num_elements % iterator::buffer_size();
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_back(1);//若符合某种条件则必须重新换一个map
		*(finish.node + 1) = allocate_node();//配置一个新节点
		construct(finish.cur, t_copy);
		finish.set_node(finish.node + 1);//改变finish，令其指向新节点
		finish.cur = finish.first;
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_front();//若符合某种条件则必须重换一个map
		*(start.node - 1) = allocate_node();//配置一个新节点（缓冲区）
		start.set_node(start.node - 1);
		start.cur = start.last - 1;
		construct(start.cur, t_copy);
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front)
	{
		size_type old_num_nodes = finish.node - start.node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		if (map_size > 2 * new_num_nodes)
		{
			new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
			if (new_nstart < start.node)
				copy(start.node, finish.node + 1, new_nstart);
			else
				copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
		}
		else
		{
			size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
			//配置一块新空间，准备给map使用
			map_pointer new_map = map_allocator::allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
			//把原内容拷贝过来
			copy(start.node, finish.node + 1, new_nstart);
			//释放原map
			map_allocator::deallocate(map, map_size);
			//设定新的map的起始地址与大小
			map = new_map;
			map_size = new_map_size;
		}
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::pop_back_aux()
	{
		deallocate_node(finish.first);//释放最后一个缓冲区
		finish.set_node(finish.node - 1);//调整finish的状态
		finish.cur = finish.last - 1;
		destory(finish.cur);
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::pop_front_aux()
	{
		destory(start.cur);
		deallocate_node(start.first);//释放第一个缓冲区
		start.set_node(start.node + 1);//调整start的状态
		start.cur = start.first;
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::clear()
	{
		//以下针对头尾以外的每一个缓冲区（它们一定都是饱和的）
		for (map_pointer node = start.node + 1; node < finish.node; ++node)
		{
			//将缓冲区全部析构。注意，调用的是destory第二版本
			destory(*node, *node + iterator::buffer_size());
			//释放缓冲区内存
			data_allocator::deallocate(*node, iterator::buffer_size());
		}
		if (start.node != finish.node)//至少有头尾两个缓冲区
		{
			destory(start.cur, start.last);
			destory(finish.cur, finish.cur);
			//以下释放尾缓冲区。注意，头缓冲区保留
			data_allocator::deallocate(finish.first, iterator::buffer_size());
		}
		else//只有一个缓冲区
			destory(start.cur, start.last);
		finish = start;//调整状态
	}
	template<typename T, typename Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator first, iterator last)
	{
		if (first == start && last == finish)//如果清除区间就是整个deque，直接调用clear()即可
		{
			clear();
			return finish;
		}
		else
		{
			difference_type n = last - first;//清除区间的长度
			difference_type elems_before = first - start;//清除区间前方的元素个数
			if (elems_before < (size() - n) / 2)
			{//如果前方的元素比较少
				copy_backward(start, first, last);
				iterator new_start = start + n;
				destory(start, new_start);
				//以下将冗余的缓冲区释放
				for (map_pointer cur = start.node; cur < new_start.node; ++cur)
				{
					data_allocator::deallocate(*cur, iterator::buffer_size());
				}
				start = new_start;//设定deque新起点
			}
			else
			{
				copy(last, finish, first);
				iterator new_finish = finish - n;
				destory(new_finish, finish);
				for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
				{
					data_allocator::deallocate(*cur, iterator::buffer_size());
				}
				finish = new_finish;
			}
			return start + elems_before;
		}
	}
	template<typename T, typename Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert(iterator position, const value_type& value)
	{
		if (position.cur == start.cur)
		{//如果插入点是deque的最前端，交给push_front去做
			push_front(value);
			return start;
		}
		else if (position.cur == finish.cur)
		{//如果插入点是deque的尾端，交给push_back去做
			push_back(value);
			iterator tmp = finish;
			--tmp;
			return tmp;
		}
		else
		{
			insert_aux(position, value);
		}
	}
	template<typename T, typename Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::insert_aux(iterator position, const value_type& value)
	{
		difference_type index = position - start;
		value_type x_copy = value;
		if (index < size() / 2)
		{//如果插入点之前的元素比较少
			push_front(front());//在最前端加入与第一元素同值的元素
			iterator front1 = start;
			++front1;
			iterator front2 = front1;
			++front2;
			position = start + index;
			iterator pos1 = position;
			++pos1;
//			copy(front2, front1, distance(front2, pos1));
//			copy(front2, pos1, front1);//元素移动
		}
		else
		{
			push_back(back());
			iterator back1 = finish;
			--back1;
			iterator back2 = back1;
			--back2;
			position = start + index;
			copy_backward(position, back2, back1);
		}
		*position = x_copy;
		return position;
	}
}


#endif