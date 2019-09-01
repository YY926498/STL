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
		//�ռ�������
		using data_allocator=simple_alloc<value_type, Alloc>;
		iterator start;//��ʾĿǰʹ�ÿռ��ͷ
		iterator finish;//��ʾĿǰʹ�ÿռ��β
		iterator end_of_storage;//��ʾĿǰ���ÿռ��β

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
		//��position��ʼ������n��Ԫ�أ�Ԫ�س�ֵΪx
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
				copy(position + 1, finish, position);//����Ԫ����ǰ�ƶ�
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
		//���ÿռ䲢��������
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
		//���б��ÿռ�
		if (finish != end_of_storage)
		{
			//�ڱ��ÿռ���ʼ������һ��Ԫ�أ�����vector���һ��Ԫ��Ϊ���ֵ
			construct(finish, *(finish - 1));
			//����ˮλ
			++finish;
			T x_copy = x;
			std::copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else//���ޱ��ÿռ�
		{
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			//��������ԭ�����ԭ��СΪ0��������1��Ԫ�ش�С
			//���ԭ��С��Ϊ0����Ϊԭ��С��2��
			//ǰ�����������ԭʼ���ݣ�����׼����������������

			iterator new_start = data_allocator::allocate(len);//ʵ������
			iterator new_finish = new_start;
			try
			{
				//��ԭvector�����ݿ�������vector
				new_finish = uninitialized_copy(start, position, new_start);
				//Ϊ��Ԫ���趨��ֵx
				construct(new_finish, x);
				//����ˮλ
				++new_finish;
				//��������ԭ����Ҳ������������ʾ�����������ܱ�insert(p,x)����
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...)
			{
				//"commit or rollback"
				destory(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			//�������ͷ�ԭvector
			destory(begin(), end());
			deallocate();
			//������������ָ����vector
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}

	template<typename T,typename Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
	{
		if (n != 0)//ֻ�е�n!=0�Ž������в���
		{
			//���ÿռ���ڡ�����Ԫ�ظ���
			if (size_type(end_of_storage - finish) >= n)
			{
				T x_copy = x;
				//���¼�������֮�������Ԫ�ظ���
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n)
				{//�����֮�������Ԫ�ش�������Ԫ�ظ���
					uninitialized_copy(finish - n, finish, finish);
					finish += n;//��vectorβ�˱�Ǻ���
					copy_backward(position, old_finish - n, old_finish);
					fill_n(position, n, x_copy);
				}
				else
				{//�����֮�������Ԫ�ظ���С�ڵ�������Ԫ�ظ���
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					std::fill(position, old_finish, x_copy);
				}
			}
			else
			{
				//���ÿռ�С�ڡ�����Ԫ�ظ�����(�Ǿͱ������¶�����ڴ�)
				//���Ⱦ����³��ȣ��ɳ��ȵ���������ɳ���+����Ԫ�ظ���
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);
				//���������µ�vector�ռ�
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try
				{
					//�������Ƚ���vector�Ĳ����֮ǰ��Ԫ�ظ��Ƶ��¿ռ�
					new_finish = uninitialized_copy(start, position, new_start);
					//�����ٽ�����Ԫ�أ���ֵΪn)�����¿ռ�
					new_finish = uninitialized_fill_n(new_finish, n, x);
					//�����ٽ���vector�Ĳ����֮���Ԫ�ظ��Ƶ��¿ռ�
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...)
				{
					//�����쳣������ʵ��"commit or rollback"
					destory(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				//����������ͷžɵ�vector
				destory(start, finish);
				deallocate();
				//���µ���ˮλ���
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
	}
}




#endif


