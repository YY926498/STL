#include "YY_allocator.h"
#include "YY_algorithm.h"
#include "YY_construct.h"
#include "YY_memory.h"
#ifndef _YY_VECTOR_H_
#define _YY_VECTOR_H_

namespace YY
{
	template<typename T,typename Alloc=alloc>
	class vector
	{
	public:
		using value_type = T;
		using pointer = value_type *;
		using iterator = value_type *;
		using reference = value_type &;
		using const_reference = const reference;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
	protected:
		//空间配置器
		using data_allocator=simple_alloc<value_type, Alloc>;
		iterator start;//表示目前使用空间的头
		iterator finish;//表示目前使用空间的尾
		iterator end_of_storage;//表示目前可用空间的尾

		void insert_aux(iterator position, const T& x);
		
		void deallocate()
		{
			if (start)
				data_allocator::deallocate(start, end_of_storage - start);
		}

		void fill_initialized(size_type n, const T& value)
		{
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}

	public:
		//从position开始，插入n个元素，元素初值为x
		void insert(iterator position, size_type n, const T& x);
		void insert(iterator position, const T& x)
		{
			insert_aux(position, x);
		}
		iterator begin()
		{
			return start;
		}
		iterator end()
		{
			return finish;
		}
		size_type size() const
		{
			return finish - start;
		}
		size_type capacity() const
		{
			return end_of_storage - start;
		}
		bool empty() const
		{
			return start==finish;
		}
		reference operator [](size_type n)
		{
			return *(begin() + n);
		}

		vector() :start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
		vector(const std::initializer_list<T>& lhs) :start(nullptr), finish(nullptr), end_of_storage(nullptr)
		{
			for (auto cur = lhs.begin(); cur != lhs.end(); ++cur)
			{
				push_back(*cur);
			}
		}
		vector(size_type n, const T& value) :start(nullptr), finish(nullptr), end_of_storage(nullptr) { fill_initialized(n, value); }
		vector(int n, const T& value) :start(nullptr), finish(nullptr), end_of_storage(nullptr) { fill_initialized(n, value); }
		vector(long n, const T& value) :start(nullptr), finish(nullptr), end_of_storage(nullptr) { fill_initialized(n, value); }
		explicit vector(size_type n) :start(nullptr), finish(nullptr), end_of_storage(nullptr) { fill_initialized(n, T()); }
		~vector()
		{
			destory(start, finish);
			deallocate();
		}

		reference front()
		{
			return *begin();
		}
		reference back()
		{
			return *(end() - 1);
		}
		void push_back(const T& x)
		{
			if (finish != end_of_storage)
			{
				construct(finish, x);
				++finish;
			}
			else
			{
				insert_aux(end(), x);
			}
		}
		void pop_back()
		{
			--finish;
			destory(finish);
		}
		iterator erase(iterator position)
		{
			if (position + 1 != end())
			{
				copy(position + 1, finish, position);//后续元素向前移动
			}
			--finish;
			destory(finish);
			return position;
		}
		iterator erase(iterator first, iterator last)
		{
			if (last != end())
			{
				copy(last, finish, first);
			}
			for (difference_type i = distance(first,last); i > 0; --i)
			{
				--finish;
				destory(finish);
			}
			return first;
		}
		void resize(size_type new_size, const T& x)
		{
			if (new_size < size())
			{
				erase(begin() + new_size, end());
			}
			else
			{
				insert(end(), new_size - size(), x);
			}
		}
		void resize(size_type new_size)
		{
			resize(new_size, T());
		}
		void clear(void)
		{
			erase(begin(), end());
		}

	protected:
		//配置空间并填满内容
		iterator allocate_and_fill(size_type n, const T& x)
		{
			iterator result = data_allocator::allocate(n);
			uninitialized_fill_n(result, n, x);
			return result;
		}

	};
	template<typename T,typename Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const T& x)
	{
		//还有备用空间
		if (finish != end_of_storage)
		{
			//在备用空间起始处构造一个元素，并以vector最后一个元素为其初值
			construct(finish, *(finish - 1));
			//调整水位
			++finish;
			T x_copy = x;
			std::copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else//已无备用空间
		{
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			//以上配置原则：如果原大小为0，则配置1个元素大小
			//如果原大小不为0，则为原大小的2倍
			//前半段用来放置原始数据，后半段准备用来放置新数据

			iterator new_start = data_allocator::allocate(len);//实际配置
			iterator new_finish = new_start;
			try
			{
				//将原vector的内容拷贝到新vector
				new_finish = uninitialized_copy(start, position, new_start);
				//为新元素设定初值x
				construct(new_finish, x);
				//调整水位
				++new_finish;
				//将安插点的原内容也拷贝过来（提示：本函数可能被insert(p,x)调用
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...)
			{
				//"commit or rollback"
				destory(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			//析构并释放原vector
			destory(begin(), end());
			deallocate();
			//调整迭代器，指向新vector
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}

	template<typename T,typename Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
	{
		if (n != 0)//只有当n!=0才进行下列操作
		{
			//备用空间大于“新增元素个数
			if (size_type(end_of_storage - finish) >= n)
			{
				T x_copy = x;
				//以下计算插入点之后的现有元素个数
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n)
				{//插入点之后的现有元素大于新增元素个数
					uninitialized_copy(finish - n, finish, finish);
					finish += n;//将vector尾端标记后移
					copy_backward(position, old_finish - n, old_finish);
					fill_n(position, n, x_copy);
				}
				else
				{//插入点之后的现有元素个数小于等于新增元素个数
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					std::fill(position, old_finish, x_copy);
				}
			}
			else
			{
				//备用空间小于“新增元素个数”(那就必须重新额外的内存)
				//首先决定新长度：旧长度的两倍，或旧长度+新增元素个数
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);
				//以下配置新的vector空间
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try
				{
					//以下首先将旧vector的插入点之前的元素复制到新空间
					new_finish = uninitialized_copy(start, position, new_start);
					//以下再将新增元素（初值为n)填入新空间
					new_finish = uninitialized_fill_n(new_finish, n, x);
					//以下再将旧vector的插入点之后的元素复制到新空间
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...)
				{
					//如有异常发生，实现"commit or rollback"
					destory(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				//以下清除并释放旧的vector
				destory(start, finish);
				deallocate();
				//以下调整水位标记
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
	}
}




#endif


