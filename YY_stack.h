#ifndef _YY_STACK_H_
#define _YY_STACK_H_


namespace YY
{
	template<typename T, typename Alloc, size_t BufSize>
	class deque;
	template<typename T,typename Sequence=YY::deque<T>>
	class stack
	{
		friend bool operator==(const stack&, const stack&);
		friend bool operator< (const stack&, const stack&);
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;

	protected:
		Sequence c;//�ײ�����
	public:
		stack() = default;
		//������ȫ����Sequence c�Ĳ��������stack�Ĳ���		
		bool empty()const
		{
			return c.empty();
		}
		size_type size() const
		{
			return c.size();
		}
		reference top()
		{
			return c.back();
		}
		const_reference top () const
		{
			return c.back();
		}
		//deque��ͷ�ɽ�����stack��ĩ�˽���ĩ�˳�
		void push(const value_type& x)
		{
			c.push_back(x);
		}
		void pop()
		{
			c.pop_back();
		}
	};
	template<typename T,typename Sequence>
	bool operator==(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
	{
		return x.c == y.c;
	}
	template<typename T,typename Sequence>
	bool operator<(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
	{
		return x.c < y.c;
	}
}




#endif
