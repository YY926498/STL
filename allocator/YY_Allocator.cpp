#include "YY_Allocator.h"

namespace YY
{
	void (*YY_malloc_alloc::_malloc_alloc_oom_handler)() = nullptr;

	void* YY_malloc_alloc::oom_malloc(size_t n)
	{
		void (*my_malloc_handler)();
		void* result;

		//不断尝试释放、配置、再释放、再配置
		for (;;)
		{
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (my_malloc_handler = nullptr)
				throw std::bad_alloc();
			(*my_malloc_handler)();//调用处理例程，企图释放内存
			result = malloc(n);//再次尝试分配内存
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
		void* result = malloc(n);//第一级配置器直接使用malloc
		//如果分配失败，改用oom_malloc
		if (result == nullptr)
			result = oom_malloc(n);
		return result;
	}

	void YY_malloc_alloc::deallocate(void* p, size_t)
	{
		free(p);//第一级配置器直接使用free
	}

	void* YY_malloc_alloc::reallocate(void* p,size_t new_sz)
	{
		void* result = realloc(p, new_sz);//第一级配置器直接使用realloc
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