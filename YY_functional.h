#ifndef _YY_FUNCTIONAL_
#define _YY_FUNCTIONAL_
namespace YY
{
	template<typename T>
	struct less
	{
		bool operator()(const T& x, const T& y)const
		{
			return x < y;
		}
	};
}

#endif
