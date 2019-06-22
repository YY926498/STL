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
		//�ó�Ա֪ͨ���������Զ���_type_traits�ػ���
		//�ı�����˵���������������������_type_traits template������ġ�
		//����Ϊ��ȷ����һ������Ҳʹ��һ����Ϊ_type_traits����ʵ��˴�����
		//�����κι���templateʱ���������鶼��˳������
		using this_dummy_member_must_be_first = _true_type;
		//��������Ӧ�ñ����أ���Ϊ�������п����Զ�Ϊ���ͱ����ר����_type_traits
		//�ػ��汾��
		//����������������µĳ�Ա����
		//������Ƴ������κγ�Ա
		//���Բ����Խ����³�Ա����������ȴû�иı�������еĶ�Ӧ����
		//�¼���ĳ�Ա����Ϊһ���Ա�������ڱ������м����ʵ�֧��
		using has_trivial_default_constructor = _false_type;
		using has_trivial_copy_constructor = _false_type;
		using has_trivial_assignment_operator = _false_type;
		using has_trivial_destructor = _false_type;
		using is_POD_type = _false_type;
	};

	//ģ���ػ�
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
	//����char��signed char��unsigned char�����ֲ�ͬ���ͣ������ڲ���ƽ̨������ͬ�ģ�
	//���Ǹ���ANSI C�Ĺ涨���������ֲ�ͬ����
	
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

	//�������ԭ��ָ�����_type_traitsƫ�ػ��汾
	//ԭ��ָ��Ҳ����Ϊһ�ֱ�������
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



