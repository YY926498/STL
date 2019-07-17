#ifndef _YY_QUEUE_H_
#define _YY_QUEUE_H_
#include "YY_deque.h"
namespace YY
{
	template<typename T,typename Sequence=deque<T>>
	class queue
	{
		friend bool operator==(const queue&, cosnt queue&);
		friend bool operator<(const queue&, const queue&);
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
	};
}



#endif
