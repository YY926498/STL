#ifndef _YY_ALLOCATOR_H_
#define _YY_ALLOCATOR_H_

#include <new>
#include <exception>
namespace YY
{
	//第一级配置器
	class YY_malloc_alloc
	{
	private:
		//处理内存不足情形
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		//处理out_of_memory异常
		static void (*_malloc_alloc_oom_handler)();
	
	public:
		static void* allocate(size_t);
		static void deallocate(void*, size_t);
		static void* reallocate(void*, size_t);

		//指定out_of_memory handler
		static void (*set_malloc_handler(void (*f)()))();
	};

	//第二级配置器
	//内存池的结点使用union，避免内存浪费
	union obj
	{
		obj* free_list_link;
		char client_data[1];
	};
	
	enum class YY_align{_ALIGN=8};//小型区块的上调边界
	enum class YY_max_bytes { _MAX_BYTES = 128};//小型区块的上限
	enum class YY_nfree
}



#endif