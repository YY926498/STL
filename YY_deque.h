#include "YY_allocator.h"
#include "YY_algorithm.h"
#include "YY_construct.h"
#include "YY_memory.h"
#ifndef _YY_DEQUE_H_
#define _YY_DEQUE_H_

namespace YY
{
	//���n��Ϊ0������n����ʾbuffer size���û��Զ���
	//���nΪ0.��ʾbuffer sizeʹ��Ĭ��ֵ����ô
	//���sz(Ԫ�ش�С��sizeof(value_type)С��512������512/sz
	//���sz��С��512������1
	inline size_t _deque_buf_size(size_t n, size_t sz)
	{
		return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
	}
	template<typename T,typename Ref,typename Ptr,size_t BufSiz>
	struct _deque_iterator
	{//δ�̳�iterator
		using iterator = _deque_iterator<T, T&, T*, BufSiz>;
		using const_iterator = _deque_iterator<T, const T&, const T*, BufSiz>;
		static size_t buffer_size()	{return _deque_buf_size(BufSiz, sizeof(T));	}
		//����δ�̳�iterator,���Ա�������׫д�����Ҫ�ĵ�������Ӧ�ͱ�
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using map_pointer = T * *;
		using self = _deque_iterator;

		//����������������
		T* cur;//�˵�������ָ֮������������Ԫ��
		T* first;//�˵�������ָ֮��������ͷ
		T* last;//�˵�������ָ֮��������β�������ÿռ䣩
		map_pointer node;//�ܿ�����

		//������һ��������
		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		//���¸���������������_deque_iterator<>�ɹ������Ĺؼ�
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
			++cur;//�л�����һԪ��
			if (cur == last)//����Ѵ����ڻ�������β��
			{
				set_node(node + 1);//���л�����һ���ڵ�(�༴������)
				cur = first;//�ĵ�һ��Ԫ��
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
		//����ʵ�������ȡ������������ֱ����Ծn������
		self& operator+=(difference_type n)
		{
			difference_type offset = n + (cur - first);
			if (offset >= 0 && offset < difference_type(buffer_size()))
			{
				//Ŀ����ͬһ��������
				cur += n;
			}
			else
			{
				//Ŀ�겻��ͬһ��������
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size())
					: -difference_type((-offset - 1) / buffer_size()) - 1;
				//�л�����ȷ�Ľڵ㣨�༴��������
				set_node(node + node_offset);
				//�л�����ȷ��Ԫ��
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}
		//ʹ��+=����������������
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
		//����ʵ�������ȡ������������ֱ����Ծn������
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
		//Ԫ��ָ���ָ��
		using map_pointer = pointer *;
		
		//���ݳ�Ա
		iterator start;//ָ���һ���ڵ�
		iterator finish;//ָ�����һ���ڵ�
		map_pointer map;//ָ��map,map��һ�������ռ䣬��ÿ��Ԫ�ض��Ǹ�ָ�룬ָ��һ���ڵ㣨��������
		size_type map_size;//map���ж���ָ��

		//ר���ռ���������ÿ������һ��Ԫ�ش�С
		using data_allocator = simple_alloc<value_type, Alloc>;
		//ר���ռ���������ÿ������һ��ָ���С
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
			//���mapβ�˵Ľڵ㱸�ÿռ䲻��
			//��������������������»�һ��map(���ø���ģ�����ԭ���ģ��ͷ�ԭ����)
			if (nodes_to_add + 1 > map_size - (finish.node - map))
				reallocate_map(nodes_to_add, false);
		}
		void reserve_map_at_front(size_type nodes_to_add = 1)
		{
			//���mapǰ�˵Ľڵ㱸�ÿռ䲻��
			//������������������ػ�һ��map
			if (nodes_to_add > start.node - map)
				reallocate_map(nodes_to_add, true);
		}
		void push_back_aux(const value_type& t);
		void push_front_aux(const value_type& t);
		//ֻ��finish.cur==finish.first�Ż����
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
			create_map_and_nodes(lhs.size());//��deque�Ľṹ�����������ź�
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
				//��󻺳�������������Ԫ�ر��ÿռ�
				construct(finish.cur, t);
				++finish.cur;
			}
			else
				push_back_aux(t);//��󻺳���ֻ��һ�����ÿռ�
		}
		void push_front(const value_type& t)
		{
			if (start.cur != start.first)
			{
				construct(start.cur - 1, t);//��һ���������п��ÿռ䣬ֱ���ڱ��ÿռ��Ϲ���Ԫ��
				--start.cur;
			}
			else
				push_front_aux(t);//��һ���������ޱ��ÿռ�
		}
		void pop_back()
		{
			if (finish.cur != finish.first)
			{
				//���һ����������һ��������ࣩԪ��
				--finish.cur;//����ָ�룬�൱���ų������Ԫ��
				destory(finish.cur);
			}
			else
				pop_back_aux();//��󻺳���û���κ�Ԫ��
		}
		void pop_front()
		{
			if (start.cur != start.last - 1)
			{
				//��һ�����������������ϵ�Ԫ��
				destory(start.cur);//����һ��Ԫ������
				++start.cur;//����ָ�룬�൱���ų��˵�һԪ��
			}
			else
				pop_front_aux();
		}
		iterator begin() { return start; }
		iterator end() { return finish; }
		reference operator[](size_type n)
		{
			return start[difference_type(n)];//����_deque_iterator<>::operator[]
		}
		reference front()
		{
			return *start;
		}
		reference back()
		{
			//iterator tmp = finish;
			//--tmp;//����_deque_iterator<>::operator--
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
		//����clear�����������deque������deque�����״̬���Ǳ���һ������������ˣ�clear��Ҫ�ָ���ʼ״̬
		void clear();
		//���pos��ָ��Ԫ�ء�posΪ�����
		iterator erase(iterator pos)
		{
			iterator next = pos;
			++next;
			difference_type index = distance(start, pos);//�����֮ǰ��Ԫ�ظ���
			if (index < (size() >> 1))//��������֮ǰ��Ԫ�ظ����Ƚ���
			{
				copy_backward(start, pos, next);//���ƶ������֮ǰ��Ԫ��
				pop_front();//�ƶ���ϣ���ǰһ��Ԫ�����࣬ȥ��֮
			}
			else
			{
				copy(next, finish, pos);
				pop_back();
			}
			return start + index;
		}
		iterator erase(iterator first, iterator last);
		//��position������һ��Ԫ�أ���ֵΪvalue
		iterator insert(iterator position, const value_type& value);
	};
	template<typename T,typename Alloc,size_t BufSize>
	void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value)
	{
		create_map_and_nodes(n);//��deque�Ľṹ�����������ź�
		map_pointer cur;
		//Ϊÿ���ڵ�Ļ������趨��ֵ
		for (cur = start.node; cur < finish.node; ++cur)
		{			
			uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
		}
		//���һ���ڵ���趨���в�ͬ(��Ϊβ�˿����б��ÿռ䣬�������ֵ)
		uninitialized_fill(finish.first, finish.cur, value);
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
	{
		//��Ҫ�ڵ���=��Ԫ�ظ���/ÿ�������������ɵ�Ԫ�ظ�����+1
		//����պ������������һ���ڵ�
		size_type num_node = num_elements / iterator::buffer_size() + 1;

		//һ��mapҪ�������ڵ㡣����8�������������ڵ�����2
		//ǰ���Ԥ��һ��������ʱ����
		map_size = max(initial_map_size(), num_node + 2);
		map = map_allocator::allocate(map_size);
		//�������ó�һ��������map_size���ڵ㡱��map

		//������nstart��nfinishָ��map��ӵ��֮ȫ���ڵ������������
		//�����������룬��ʹͷβ���˵���������һ����ÿ���ڵ�ɶ�Ӧһ��������
		map_pointer nstart = map + (map_size - num_node) / 2;
		map_pointer nfinish = nstart + num_node - 1;

		map_pointer cur;
		//Ϊmap�ڵ�ÿ�����ýڵ����û����������л���������������deque�Ŀ��ÿռ�
		//�����һ����������������һЩ��ԣ��
		//�˴�δ�����쳣
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
		reserve_map_at_back(1);//������ĳ��������������»�һ��map
		*(finish.node + 1) = allocate_node();//����һ���½ڵ�
		construct(finish.cur, t_copy);
		finish.set_node(finish.node + 1);//�ı�finish������ָ���½ڵ�
		finish.cur = finish.first;
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_front();//������ĳ������������ػ�һ��map
		*(start.node - 1) = allocate_node();//����һ���½ڵ㣨��������
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
			//����һ���¿ռ䣬׼����mapʹ��
			map_pointer new_map = map_allocator::allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
			//��ԭ���ݿ�������
			copy(start.node, finish.node + 1, new_nstart);
			//�ͷ�ԭmap
			map_allocator::deallocate(map, map_size);
			//�趨�µ�map����ʼ��ַ���С
			map = new_map;
			map_size = new_map_size;
		}
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::pop_back_aux()
	{
		deallocate_node(finish.first);//�ͷ����һ��������
		finish.set_node(finish.node - 1);//����finish��״̬
		finish.cur = finish.last - 1;
		destory(finish.cur);
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::pop_front_aux()
	{
		destory(start.cur);
		deallocate_node(start.first);//�ͷŵ�һ��������
		start.set_node(start.node + 1);//����start��״̬
		start.cur = start.first;
	}
	template<typename T, typename Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::clear()
	{
		//�������ͷβ�����ÿһ��������������һ�����Ǳ��͵ģ�
		for (map_pointer node = start.node + 1; node < finish.node; ++node)
		{
			//��������ȫ��������ע�⣬���õ���destory�ڶ��汾
			destory(*node, *node + iterator::buffer_size());
			//�ͷŻ������ڴ�
			data_allocator::deallocate(*node, iterator::buffer_size());
		}
		if (start.node != finish.node)//������ͷβ����������
		{
			destory(start.cur, start.last);
			destory(finish.cur, finish.cur);
			//�����ͷ�β��������ע�⣬ͷ����������
			data_allocator::deallocate(finish.first, iterator::buffer_size());
		}
		else//ֻ��һ��������
			destory(start.cur, start.last);
		finish = start;//����״̬
	}
	template<typename T, typename Alloc, size_t BufSize>
	typename deque<T, Alloc, BufSize>::iterator deque<T, Alloc, BufSize>::erase(iterator first, iterator last)
	{
		if (first == start && last == finish)//�����������������deque��ֱ�ӵ���clear()����
		{
			clear();
			return finish;
		}
		else
		{
			difference_type n = last - first;//�������ĳ���
			difference_type elems_before = first - start;//�������ǰ����Ԫ�ظ���
			if (elems_before < (size() - n) / 2)
			{//���ǰ����Ԫ�رȽ���
				copy_backward(start, first, last);
				iterator new_start = start + n;
				destory(start, new_start);
				//���½�����Ļ������ͷ�
				for (map_pointer cur = start.node; cur < new_start.node; ++cur)
				{
					data_allocator::deallocate(*cur, iterator::buffer_size());
				}
				start = new_start;//�趨deque�����
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
		{//����������deque����ǰ�ˣ�����push_frontȥ��
			push_front(value);
			return start;
		}
		else if (position.cur == finish.cur)
		{//����������deque��β�ˣ�����push_backȥ��
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
		{//��������֮ǰ��Ԫ�رȽ���
			push_front(front());//����ǰ�˼������һԪ��ֵͬ��Ԫ��
			iterator front1 = start;
			++front1;
			iterator front2 = front1;
			++front2;
			position = start + index;
			iterator pos1 = position;
			++pos1;
//			copy(front2, front1, distance(front2, pos1));
//			copy(front2, pos1, front1);//Ԫ���ƶ�
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