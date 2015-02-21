#include <string.h>
#include "LBS.h"
#include "MEM.h"
#include "DBG.h"
#include "UTL.h"
#include "SandBox_pri.h"

TypeInfo Loopr_Type_Info[] = {
	{"dummy",	"dummy", 		NULL,		-1},
	{"null",	"Null",			NULL,		0},

	{"bl",		"Boolean",		"%d",		sizeof(Loopr_Boolean)},
	{"c32",		"Char",			"%u",		sizeof(Loopr_Char)},

	{"s8",		"SByte",		"%d",		sizeof(Loopr_SByte)},
	{"b8",		"Byte",			"%u",		sizeof(Loopr_Byte)},

	{"i16",		"Int16",		"%d",		sizeof(Loopr_Int16)},
	{"u16",		"UInt16",		"%u",		sizeof(Loopr_UInt16)},

	{"i32",		"Int32",		"%d",		sizeof(Loopr_Int32)},
	{"u32",		"UInt32",		"%u",		sizeof(Loopr_UInt32)},

	{"i64",		"Int64",		"%ld",		sizeof(Loopr_Int64)},
	{"u64",		"UInt64",		"%lu",		sizeof(Loopr_UInt64)},

	{"f8",		"Single",		"%f",		sizeof(Loopr_Double)}, /* Convert to double */
	{"f16",		"Double",		"%lf",		sizeof(Loopr_Double)},
	{"str",		"String",		"%s",		sizeof(Loopr_Char *)},

	{"obj",		"Object",		NULL,		sizeof(Loopr_Value *)},
	{"arr",		"Array",		NULL,		sizeof(Loopr_Array)},
};

Loopr_Byte *
Loopr_byte_serialize(const void *data, int length)
{
	Loopr_Byte *ret;

	ret = MEM_malloc(sizeof(Loopr_Byte) * length);
	memcpy(ret, data, length);

	return ret;
}

Loopr_Ref *
Loopr_alloc_ref(Loopr_BasicType type)
{
	Loopr_Ref *ret;

	ret = MEM_malloc(sizeof(Loopr_Ref));
	ret->marked = 0;

	ret->type = type;

	ret->prev = NULL;
	ret->next = NULL;
	Walle_add_object(ret);

	return ret;
}

Loopr_Ref *
Loopr_create_string(Loopr_Byte *data, int *offset)
{
	int length;
	Loopr_Ref *ret;

	length = Loopr_mbstowcs_len((char *)data);
	*offset = strlen((char *)data) + 1;

	ret = Loopr_alloc_ref(LPR_STRING);

	ret->u.string_value = MEM_malloc(sizeof(Loopr_Char) * (length + 1));
	Loopr_mbstowcs((char *)data, ret->u.string_value);

	return ret;
}

Loopr_Char *
Loopr_conv_string(Loopr_Byte *data)
{
	int length;
	Loopr_Char *ret;

	length = Loopr_mbstowcs_len((char *)data);

	ret = MEM_malloc(sizeof(Loopr_Char) * (length + 1));
	Loopr_mbstowcs((char *)data, ret);

	return ret;
}

Loopr_Ref *
Loopr_create_object(Loopr_Value orig, Loopr_Boolean ref_flag)
{
	Loopr_Ref *ret;

	ret = Loopr_alloc_ref(LPR_OBJECT);
	ret->u.object_value.value = orig;
	ret->u.object_value.ref_flag = ref_flag;

	return ret;
}

Loopr_Value
Loopr_get_init_value(Loopr_BasicType type)
{
	/*Loopr_Value *value;

	switch (type) {
		case LPR_BOOLEAN:
		case LPR_CHAR:
		case LPR_SBYTE:
		case LPR_INT16:
		case LPR_INT32:
		case LPR_INT64:
		case LPR_BYTE:
		case LPR_UINT16:
		case LPR_UINT32:
		case LPR_UINT64:
			value = Loopr_alloc_value(type);
			value->u.uint64_value = 0;
			break;
		case LPR_SINGLE:
		case LPR_DOUBLE:
			value = Loopr_alloc_value(type);
			value->u.double_value = 0.0;
			break;
		default:
			value = Loopr_create_null();
			break;
	}*/
	Loopr_Value value;
	value.ref_value = NULL;

	return value;
}
