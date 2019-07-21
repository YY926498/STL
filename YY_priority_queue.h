#ifndef _YY_PRIORITY_QUEUE_
#define _YY_PRIORITY_QUEUE_
#include "YY_vector.h"
#include "YY_functional.h"
#include "YY_algorithm.h"
namespace YY
{
	template<typename T,typename Sequence=vector<T>,typename Compare = less<typename Sequence::value_type>>
	class priority_queue
	{
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
	protected:
		Sequence c;//底层容器
		Compare comp;//元素大小比较标准
	public:
		priority_queue() :c() {}
		explicit priority_queue(const Compare& x) :c(), comp(x) {}
		//以下用到的make_heap(),push_heap(),pop_heap()都是泛型算法
		//注意，任意一个构造函数都立刻于底层容器中产生一个implicit representation heap
		template<typename InputIterator>
		priority_queue(InputIterator first, InputIterator last, const Compare& x) : c(first, last), comp(x)
		{
			make_heap(c.begin(), c.end(), comp);
		}
		template<typename InputIterator>
		priority_queue(InputIterator first, InputIterator last) : c(first, last)
		{
			make_heap(c.begin(), c.end(), comp);
		}
		bool empty() const
		{
			return c.empty();
		}
		size_type size() const
		{
			return c.size();
		}
		const_reference top() const
		{
			return c.front();
		}
		reference top()
		{
			return c.front();
		}
		void push(const value_type& x)
		{
			c.push_back(x);
			push_heap(c.begin(), c.end(), comp);
		}
		void pop()
		{
			pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}
	};
}






#endif

