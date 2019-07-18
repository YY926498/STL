#ifndef _YY_QUEUE_H_
#define _YY_QUEUE_H_
#include "YY_deque.h"
namespace YY
{
	template<typename T,typename Sequence=deque<T>>
	class queue
	{
		friend bool operator==(const queue&, const queue&);
		friend bool operator<(const queue&, const queue&);
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
	protected:
		Sequence c;//�ײ�����
	public://������ȫ����Sequence c�Ĳ��������queue�Ĳ���
		queue() = default;
		bool empty()const
		{
			return c.empty();
		}
		size_type size() const
		{
			return c.size();
		}
		reference front()
		{
			return c.front();
		}
		const_reference front()const
		{
			return c.front();
		}
		reference back()
		{
			return c.back();
		}
		const_reference back()const
		{
			return c.back();
		}
		//deque����ͷ�ɽ�����queue���Ƚ��ȳ�
		void push(const value_type& x)
		{
			c.push_back(x);
		}
		void pop()
		{
			c.pop_front();
		}
	};
	template<typename T, typename Sequence>
	bool operator == (const queue<T, Sequence>& x, const queue<T, Sequence>& y)
	{
		return x.c == y.c;
	}
	template<typename T,typename Sequence>
	bool operator < (const queue<T, Sequence>& x, const queue<T, Sequence>& y)
	{
		return x.c < y.c;
	}
}



#endif

