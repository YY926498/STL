#ifndef _YY_TYPE_TRAITS_
#define _YY_TYPE_TRAITS_


namespace YY
{
	#define _YY_TEMPLATE_NULL template<>
	struct _true_type{};
	struct _false_type{};
	template<typename type>
	struct _type_traits
	{
		//该成员通知“有能力自动将_type_traits特化”
		//的编译器说，我们现在所看到的这个_type_traits template是特殊的。
		//这是为了确保万一编译器也使用一个名为_type_traits而其实与此处定义
		//并无任何关联template时，所有事情都能顺利运作
		using this_dummy_member_must_be_first = _true_type;
		//以下条件应该被遵守，因为编译器有可能自动为各型别产生专属的_type_traits
		//特化版本：
		//你可以重新排列以下的成员次序
		//你可以移除以下任何成员
		//绝对不可以将以下成员重新命名而却没有改变编译器中的对应名称
		//新加入的成员被视为一般成员，除非在编译器中加入适当支持
		using has_trivial_default_constructor = _false_type;
		using has_trivial_copy_constructor = _false_type;
		using has_trivial_assignment_operator = _false_type;
		using has_trivial_destructor = _false_type;
		using is_POD_type = _false_type;
	};

	//模板特化
	_YY_TEMPLATE_NULL struct _type_traits<char>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<signed char>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<unsigned char>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};
	//由于char、signed char、unsigned char是三种不同类型，可能在部分平台上是相同的，
	//但是根据ANSI C的规定，属于三种不同类型
	
	_YY_TEMPLATE_NULL struct _type_traits<short>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<unsigned short>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<int>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<unsigned int>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<long>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<unsigned long>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<float>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<double>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	_YY_TEMPLATE_NULL struct _type_traits<long double>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};

	//以下针对原生指针设计_type_traits偏特化版本
	//原生指针也被视为一种标量类型
	template<typename T>
	struct _type_traits<T*>
	{
		using has_trivial_default_constructor = _true_type;
		using has_trivial_copy_constructor = _true_type;
		using has_trivial_assignment_operator = _true_type;
		using has_trivial_destructor = _true_type;
		using is_POD_type = _true_type;
	};
}



#endif



