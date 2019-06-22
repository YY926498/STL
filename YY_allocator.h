#ifndef _YY_ALLOCATOR_H_
#define _YY_ALLOCATOR_H_

#include <new>
#include <exception>
#define _YY_THROW_BAD_ALLOC throw std::bad_alloc{}


namespace YY
{
	template<int inst>
	class _malloc_alloc_template
	{
	private:
		//以下函数用于处理内存不足的情况
		//oom: out of memory
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void (*_malloc_alloc_oom_handle)();

	public:
		static void* allocate(size_t n)
		{
			void* result = malloc(n);//第一级配置器直接使用malloc
			//无法满足要求时，改用oom_malloc
			if (result == nullptr)
				result = oom_malloc(n);
			return result;
		}

		static void deallocate(void* p, size_t)
		{
			free(p);//第一级配置器直接使用free()
		}

		static void* reallocate(void* p, size_t old_sz, size_t new_sz)
		{
			void* result = realloc(p, new_sz);//第一级配置器直接使用realloc
			//无法满足要求时，改用oom_realloc
			if (result == nullptr)
				result = oom_realloc(p, new_sz);
			return result;
		}

		//指定oom_handler
		static void (*set_malloc_handler(void (*f)()))()
		{
			void (*old)() = _malloc_alloc_oom_handle;
			_malloc_alloc_oom_handle = f;
			return (old);
		}
	};

	//malloc_alloc oom_handler默认初值为nullptr

	template<int inst>
	void (*_malloc_alloc_template<inst>::_malloc_alloc_oom_handle)() = nullptr;

	template<int inst>
	void* _malloc_alloc_template<inst>::oom_malloc(size_t n)
	{
		void (*my_malloc_handler)();
		void* result;
		while (1)
		{
			my_malloc_handler = _malloc_alloc_oom_handle;
			if (my_malloc_handler == nullptr)
			{
				_YY_THROW_BAD_ALLOC;
			}
			(*my_malloc_handler)();
			result = malloc(n);
			if (result)
				return result;
		}
	}

	template<int inst>
	void* _malloc_alloc_template<inst>::oom_realloc(void* p, size_t n)
	{
		void(*my_malloc_handler)();
		void* result;

		while (1)
		{
			my_malloc_handler = _malloc_alloc_oom_handle;
			if (my_malloc_handler == nullptr)
				_YY_THROW_BAD_ALLOC;
			(*my_malloc_handler)();
			result = realloc(p, n);
			if (result)
				return result;
		}
	}
	using malloc_alloc=_malloc_alloc_template<0>;

	enum {_ALIGN=8};//小型区块的上调边界
	enum {_MAX_BYTES=128};//小型区块的上限
	enum {_NFREELISTS=_MAX_BYTES/_ALIGN};//free-lists个数

	//第二级配置器
	template<bool threads,int inst>
	class _default_alloc_tempalte
	{
	private:
		//ROUND_UP将Bytes上调至8的倍数
		static size_t ROUND_UP(size_t bytes)
		{
			return ((bytes)+_ALIGN - 1) & ~(_ALIGN - 1);
		}
		union obj//free-list的节点构造
		{
			union obj* free_list_link;
			char client_data[1];
		};
		
		//16个free-lists
		static obj* volatile free_list[_NFREELISTS];
		//以下函数根据区块大小，决定使用第n好free-list。n从0开始
		static size_t FREELIST_INDEX(size_t bytes)
		{
			return (((bytes)+_ALIGN - 1) / _ALIGN - 1);
		}

		//返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
		static void* refill(size_t n);
		
		//配置一大块空间，可容纳nobjs个大小为"size"的区块
		//如果配置objs个区块有所不便，nobjs可能会降低
		static char* chunk_alloc(size_t size, int& nobjs);

		//Chunk allocation state
		static char* start_free;//内存池起始地址，只在chunk_alloc()中变化
		static char* end_free;//内存池结束地址，只在chunk_alloc()中变化
		static size_t heap_size;

	public:
		static void* allocate(size_t n);
		static void deallocate(void* p, size_t n);
		//static void* reallocate(void* p, size_t old_sz, size_t new_sz);
	};

	//以下是static data member的定义与初值设定
	template<bool threads,int inst>
	char* _default_alloc_tempalte<threads, inst>::start_free = nullptr;
	
	template<bool threads, int inst>
	char* _default_alloc_tempalte<threads, inst>::end_free = nullptr;

	template<bool threads, int inst>
	size_t _default_alloc_tempalte<threads, inst>::heap_size = 0;

	template<bool threads, int inst>
	 typename _default_alloc_tempalte<threads, inst>::obj* volatile
		_default_alloc_tempalte<threads, inst>::free_list[_NFREELISTS] = { 0 };

	 template<bool threads, int inst>
	 void* _default_alloc_tempalte<threads, inst>::allocate(size_t n)
	 {
		 obj* volatile* my_free_list;
		 obj* result;

		 //如果大于128就调用第一级配置器
		 if (n > (size_t)_MAX_BYTES)
			 return (malloc_alloc::allocate(n));
		 //寻找16个free-list中适当的一个
		 my_free_list = free_list + FREELIST_INDEX(n);
		 result = *my_free_list;
		 if (result == nullptr)
		 {
			 //没找到可用的free list，准备重新填充free list
			 void* r = refill(ROUND_UP(n));
			 return r;
		 }
		 //调整free list
		 *my_free_list = result->free_list_link;
		 return (result);
	 }

	 //p不可以是nullptr
	 template<bool threads, int inst>
	 void _default_alloc_tempalte<threads, inst>::deallocate(void* p, size_t n)
	 {
		 obj* q = (obj*)p;
		 obj* volatile* my_free_list;

		 if (n > (size_t)_MAX_BYTES)
		 {
			 malloc_alloc::deallocate(p, n);
			 return;
		 }
		 //寻找对应的free list
		 my_free_list = free_list + FREELIST_INDEX(n);
		 //调整free list，回收区块
		 q->free_list_link = *my_free_list;
		 *my_free_list = q;
	 }

	 template<bool threads, int inst>
	 void* _default_alloc_tempalte<threads, inst>::refill(size_t n)
	 {
		 int nobjs = 20;
		 //调用chunk_alloc()，尝试取得nobjs个区块作为free list的新节点
		 char* chunk = chunk_alloc(n, nobjs);
		 obj* volatile* my_free_list;
		 obj* result;
		 obj* current_obj, * next_obj;
		 int i;
		 //如果只获得一个区块，这个区块就分配给调用者，free list无新节点
		 if (nobjs == 1)
			 return chunk;

		 my_free_list = free_list + FREELIST_INDEX(n);

		 //以下在chunk空间内建立free list
		 result = (obj*)chunk;//这一块准备返回给客户
		 //以下导引free list指向新配置的空间（取自内存池）
		 *my_free_list = next_obj = (obj*)(chunk + n);
		 //以下将free list的各节点串接起来
		 for (i = 1;; i++)
		 {
			 current_obj = next_obj;
			 next_obj = (obj*)((char*)next_obj + n);
			 if (nobjs - 1 == i)
			 {
				 current_obj->free_list_link = 0;
				 break;
			 }
			 else
			 {
				 current_obj->free_list_link = next_obj;
			 }
		 }
		 return (result);
	 }
	 
	 //假设size已经适当上调至8的倍数
	 template<bool threads, int inst>
	 char* _default_alloc_tempalte<threads, inst>::chunk_alloc(size_t size, int& nobjs)
	 {
		 char* result;
		 size_t total_bytes = size * nobjs;
		 size_t bytes_left = end_free - start_free;//内存池剩余容量

		 if (bytes_left >= total_bytes)
		 {
			 //内存池剩余空间完全满足需求量
			 result = start_free;
			 start_free += total_bytes;
			 return (result);
		 }
		 else if (bytes_left >= size)
		 {
			 //内存池剩余空间不能完全满足需求量，但足够供应一个（含）以上的区块
			 nobjs = bytes_left / size;
			 total_bytes = size * nobjs;
			 result = start_free;
			 start_free += total_bytes;
			 return (result);
		 }
		 else
		 {
			 //内存池剩余空间连一个区块的大小都无法提供
			 size_t bytes_to_get = total_bytes * 2+ ROUND_UP((size_t)heap_size >> 4);
			 //以下试着让内存池中的残余零头还有利用价值
			 if (bytes_left > 0)
			 {
				 //内存池还有一些零头，先分配给适当的free list
				 //首先寻找适当的free list
				 obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
				 //调整free list,将内存池中的残余空间编入
				 ((obj*)start_free)->free_list_link = *my_free_list;
				 *my_free_list = (obj*)start_free;
			 }

			 //配置heap空间
			 start_free = (char*)malloc(bytes_to_get);
			 if (start_free == nullptr)
			 {
				 //heap空间不足，malloc()失败
				 int i;
				 obj* volatile* my_free_list, * p;
				 //尝试检视拥有的资源，这不会造成伤害，因为不打算配置
				 //较小的区块，因为在多进程机器上容易造成灾难
				 //以下搜寻适当的free list
				 //所谓适当是指“尚有可用区块，且区块足够大”之free list
				 for (i = size; i <= _MAX_BYTES; i += _ALIGN)
				 {
					 my_free_list = free_list + FREELIST_INDEX(i);
					 p = *my_free_list;
					 if (p != nullptr)//free list内尚有可用区块，调整free list以释出未用区块
					 {
						 *my_free_list = p->free_list_link;
						 start_free = (char*)p;
						 end_free = start_free + i;
						 //递归调用自己，为了修正nobjs
						 return (chunk_alloc(size, nobjs));
						 //注意，将残余零头终将编入适当的free list中备用
					 }
				 }

				 end_free = nullptr;//如果出现意外，调用第一级配置器，看看out of memory机制能否尽点力
				 start_free = (char*)malloc_alloc::allocate(bytes_to_get);
				 //这会导致抛出异常，或内存不足的情况获得改善
			 }
				 heap_size += bytes_to_get;
				 end_free = start_free + bytes_to_get;
				 //递归调用自己，修正nobjs
				 return (chunk_alloc(size, nobjs));
			 
		 }
	 }
	 using alloc=_default_alloc_tempalte<false, 1>;

	 template<typename T, typename Alloc>
	 class simple_alloc
	 {
	 public:
		 static T* allocate(size_t n)
		 {
			 return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
		 }
		 static T* allocate(void)
		 {
			 return (T*)Alloc::allocate(sizeof(T));
		 }
		 static void deallocate(T* p, size_t n)
		 {
			 if (0 != n)
				 Alloc::deallocate(p, n * sizeof(T));
		 }
		 static void deallocate(T* p)
		 {
			 Alloc::deallocate(p, sizeof(T));
		 }
	 };
}





#endif // !_YY_ALLOCATOR_H_
