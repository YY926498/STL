#ifndef _YY_ALLOCATOR_H_
#define _YY_ALLOCATOR_H_

#include <new>
#include <exception>
namespace YY
{
	//��һ��������
	class YY_malloc_alloc
	{
	private:
		//�����ڴ治������
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		//����out_of_memory�쳣
		static void (*_malloc_alloc_oom_handler)();
	
	public:
		static void* allocate(size_t);
		static void deallocate(void*, size_t);
		static void* reallocate(void*, size_t);

		//ָ��out_of_memory handler
		static void (*set_malloc_handler(void (*f)()))();
	};

	//�ڶ���������
	//�ڴ�صĽ��ʹ��union�������ڴ��˷�
	union obj
	{
		obj* free_list_link;
		char client_data[1];
	};
	
	enum class YY_align{_ALIGN=8};//С��������ϵ��߽�
	enum class YY_max_bytes { _MAX_BYTES = 128};//С�����������
	enum class YY_nfree
}



#endif