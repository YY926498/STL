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
		//���º������ڴ����ڴ治������
		//oom: out of memory
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void (*_malloc_alloc_oom_handle)();

	public:
		static void* allocate(size_t n)
		{
			void* result = malloc(n);//��һ��������ֱ��ʹ��malloc
			//�޷�����Ҫ��ʱ������oom_malloc
			if (result == nullptr)
				result = oom_malloc(n);
			return result;
		}

		static void deallocate(void* p, size_t)
		{
			free(p);//��һ��������ֱ��ʹ��free()
		}

		static void* reallocate(void* p, size_t old_sz, size_t new_sz)
		{
			void* result = realloc(p, new_sz);//��һ��������ֱ��ʹ��realloc
			//�޷�����Ҫ��ʱ������oom_realloc
			if (result == nullptr)
				result = oom_realloc(p, new_sz);
			return result;
		}

		//ָ��oom_handler
		static void (*set_malloc_handler(void (*f)()))()
		{
			void (*old)() = _malloc_alloc_oom_handle;
			_malloc_alloc_oom_handle = f;
			return (old);
		}
	};

	//malloc_alloc oom_handlerĬ�ϳ�ֵΪnullptr

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

	enum {_ALIGN=8};//С��������ϵ��߽�
	enum {_MAX_BYTES=128};//С�����������
	enum {_NFREELISTS=_MAX_BYTES/_ALIGN};//free-lists����

	//�ڶ���������
	template<bool threads,int inst>
	class _default_alloc_tempalte
	{
	private:
		//ROUND_UP��Bytes�ϵ���8�ı���
		static size_t ROUND_UP(size_t bytes)
		{
			return ((bytes)+_ALIGN - 1) & ~(_ALIGN - 1);
		}
		union obj//free-list�Ľڵ㹹��
		{
			union obj* free_list_link;
			char client_data[1];
		};
		
		//16��free-lists
		static obj* volatile free_list[_NFREELISTS];
		//���º������������С������ʹ�õ�n��free-list��n��0��ʼ
		static size_t FREELIST_INDEX(size_t bytes)
		{
			return (((bytes)+_ALIGN - 1) / _ALIGN - 1);
		}

		//����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free-list
		static void* refill(size_t n);
		
		//����һ���ռ䣬������nobjs����СΪ"size"������
		//�������objs�������������㣬nobjs���ܻή��
		static char* chunk_alloc(size_t size, int& nobjs);

		//Chunk allocation state
		static char* start_free;//�ڴ����ʼ��ַ��ֻ��chunk_alloc()�б仯
		static char* end_free;//�ڴ�ؽ�����ַ��ֻ��chunk_alloc()�б仯
		static size_t heap_size;

	public:
		static void* allocate(size_t n);
		static void deallocate(void* p, size_t n);
		//static void* reallocate(void* p, size_t old_sz, size_t new_sz);
	};

	//������static data member�Ķ������ֵ�趨
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

		 //�������128�͵��õ�һ��������
		 if (n > (size_t)_MAX_BYTES)
			 return (malloc_alloc::allocate(n));
		 //Ѱ��16��free-list���ʵ���һ��
		 my_free_list = free_list + FREELIST_INDEX(n);
		 result = *my_free_list;
		 if (result == nullptr)
		 {
			 //û�ҵ����õ�free list��׼���������free list
			 void* r = refill(ROUND_UP(n));
			 return r;
		 }
		 //����free list
		 *my_free_list = result->free_list_link;
		 return (result);
	 }

	 //p��������nullptr
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
		 //Ѱ�Ҷ�Ӧ��free list
		 my_free_list = free_list + FREELIST_INDEX(n);
		 //����free list����������
		 q->free_list_link = *my_free_list;
		 *my_free_list = q;
	 }

	 template<bool threads, int inst>
	 void* _default_alloc_tempalte<threads, inst>::refill(size_t n)
	 {
		 int nobjs = 20;
		 //����chunk_alloc()������ȡ��nobjs��������Ϊfree list���½ڵ�
		 char* chunk = chunk_alloc(n, nobjs);
		 obj* volatile* my_free_list;
		 obj* result;
		 obj* current_obj, * next_obj;
		 int i;
		 //���ֻ���һ�����飬�������ͷ���������ߣ�free list���½ڵ�
		 if (nobjs == 1)
			 return chunk;

		 my_free_list = free_list + FREELIST_INDEX(n);

		 //������chunk�ռ��ڽ���free list
		 result = (obj*)chunk;//��һ��׼�����ظ��ͻ�
		 //���µ���free listָ�������õĿռ䣨ȡ���ڴ�أ�
		 *my_free_list = next_obj = (obj*)(chunk + n);
		 //���½�free list�ĸ��ڵ㴮������
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
	 
	 //����size�Ѿ��ʵ��ϵ���8�ı���
	 template<bool threads, int inst>
	 char* _default_alloc_tempalte<threads, inst>::chunk_alloc(size_t size, int& nobjs)
	 {
		 char* result;
		 size_t total_bytes = size * nobjs;
		 size_t bytes_left = end_free - start_free;//�ڴ��ʣ������

		 if (bytes_left >= total_bytes)
		 {
			 //�ڴ��ʣ��ռ���ȫ����������
			 result = start_free;
			 start_free += total_bytes;
			 return (result);
		 }
		 else if (bytes_left >= size)
		 {
			 //�ڴ��ʣ��ռ䲻����ȫ���������������㹻��Ӧһ�����������ϵ�����
			 nobjs = bytes_left / size;
			 total_bytes = size * nobjs;
			 result = start_free;
			 start_free += total_bytes;
			 return (result);
		 }
		 else
		 {
			 //�ڴ��ʣ��ռ���һ������Ĵ�С���޷��ṩ
			 size_t bytes_to_get = total_bytes * 2+ ROUND_UP((size_t)heap_size >> 4);
			 //�����������ڴ���еĲ�����ͷ�������ü�ֵ
			 if (bytes_left > 0)
			 {
				 //�ڴ�ػ���һЩ��ͷ���ȷ�����ʵ���free list
				 //����Ѱ���ʵ���free list
				 obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
				 //����free list,���ڴ���еĲ���ռ����
				 ((obj*)start_free)->free_list_link = *my_free_list;
				 *my_free_list = (obj*)start_free;
			 }

			 //����heap�ռ�
			 start_free = (char*)malloc(bytes_to_get);
			 if (start_free == nullptr)
			 {
				 //heap�ռ䲻�㣬malloc()ʧ��
				 int i;
				 obj* volatile* my_free_list, * p;
				 //���Լ���ӵ�е���Դ���ⲻ������˺�����Ϊ����������
				 //��С�����飬��Ϊ�ڶ���̻����������������
				 //������Ѱ�ʵ���free list
				 //��ν�ʵ���ָ�����п������飬�������㹻��֮free list
				 for (i = size; i <= _MAX_BYTES; i += _ALIGN)
				 {
					 my_free_list = free_list + FREELIST_INDEX(i);
					 p = *my_free_list;
					 if (p != nullptr)//free list�����п������飬����free list���ͳ�δ������
					 {
						 *my_free_list = p->free_list_link;
						 start_free = (char*)p;
						 end_free = start_free + i;
						 //�ݹ�����Լ���Ϊ������nobjs
						 return (chunk_alloc(size, nobjs));
						 //ע�⣬��������ͷ�ս������ʵ���free list�б���
					 }
				 }

				 end_free = nullptr;//����������⣬���õ�һ��������������out of memory�����ܷ񾡵���
				 start_free = (char*)malloc_alloc::allocate(bytes_to_get);
				 //��ᵼ���׳��쳣�����ڴ治��������ø���
			 }
				 heap_size += bytes_to_get;
				 end_free = start_free + bytes_to_get;
				 //�ݹ�����Լ�������nobjs
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
