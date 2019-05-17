#include "YY_Allocator.h"

namespace YY
{
	void (*YY_malloc_alloc::_malloc_alloc_oom_handler)() = nullptr;

	void* YY_malloc_alloc::oom_malloc(size_t n)
	{
		void (*my_malloc_handler)();
		void* result;

		//���ϳ����ͷš����á����ͷš�������
		for (;;)
		{
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (my_malloc_handler = nullptr)
				throw std::bad_alloc();
			(*my_malloc_handler)();//���ô������̣���ͼ�ͷ��ڴ�
			result = malloc(n);//�ٴγ��Է����ڴ�
			if (result)
				return (result);
		}
	}

	void* YY_malloc_alloc::oom_realloc(void* p, size_t n)
	{
		void (*my_malloc_handler)();
		void* result;

		for (;;)
		{
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw std::bad_alloc();
			(*my_malloc_handler)();
			result = realloc(p, n);
			if (result)
				return (result);
		}
	}

	void* YY_malloc_alloc::allocate(size_t n)
	{
		void* result = malloc(n);//��һ��������ֱ��ʹ��malloc
		//�������ʧ�ܣ�����oom_malloc
		if (result == nullptr)
			result = oom_malloc(n);
		return result;
	}

	void YY_malloc_alloc::deallocate(void* p, size_t)
	{
		free(p);//��һ��������ֱ��ʹ��free
	}

	void* YY_malloc_alloc::reallocate(void* p,size_t new_sz)
	{
		void* result = realloc(p, new_sz);//��һ��������ֱ��ʹ��realloc
		if (result == nullptr)
			result = oom_realloc(p, new_sz);
		return result;
	}

	void (*YY_malloc_alloc::set_malloc_handler(void (*f)()))()
	{
		void (*old_handler)() = _malloc_alloc_oom_handler;
		_malloc_alloc_oom_handler = f;
		return (old_handler);
	}
}