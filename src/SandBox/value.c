#include <string.h>
#include "SandBox_pri.h"
#include "MEM.h"
#include "DBG.h"
#include "UTL.h"

TypeInfo Loopr_Type_Info[] = {
	{"dummy",	"dummy", 		"",			-1},
	{"null",	"Null",			"",			0},

	{"bl",		"Boolean",		"%d",		sizeof(Loopr_Boolean)},
	{"c4",		"Char",			"%u",		sizeof(Loopr_Char)},

	{"s8",		"SByte",		"%d",		sizeof(Loopr_SByte)},
	{"b8",		"Byte",			"%u",		sizeof(Loopr_Byte)},

	{"i16",		"Int16",		"%d",		sizeof(Loopr_Int16)},
	{"u16",		"UInt16",		"%u",		sizeof(Loopr_UInt16)},

	{"i32",		"Int32",		"%d",		sizeof(Loopr_Int32)},
	{"u32",		"UInt32",		"%u",		sizeof(Loopr_UInt32)},

	{"i64",		"Int64",		"%ld",		sizeof(Loopr_Int64)},
	{"u64",		"UInt64",		"%lu",		sizeof(Loopr_UInt64)},

	{"f8",		"Single",		"%f",		sizeof(Loopr_Single)},
	{"f16",		"Double",		"%lf",		sizeof(Loopr_Double)},
	{"str",		"String",		"%s",		sizeof(Loopr_Char *)},

	{"obj",		"Object",		"",			sizeof(Loopr_Value *)},
};

Loopr_Byte *
Loopr_byte_serialize(const void *data, int length)
{
	Loopr_Byte *ret;

	ret = MEM_malloc(sizeof(Loopr_Byte) * length);
	memcpy(ret, data, length);

	return ret;
}

Loopr_InfoTable *
Loopr_alloc_info_table(Loopr_BasicType type)
{
	Loopr_InfoTable *ret;

	ret = MEM_malloc(sizeof(Loopr_InfoTable));
	ret->type = type;

	return ret;
}

Loopr_Value *
Loopr_alloc_value(Loopr_BasicType type)
{
	Loopr_Value *ret;

	ret = MEM_malloc(sizeof(Loopr_Value));
	ret->type = type;
	ret->u.double_value = NULL_VALUE;

	ret->marked = 0;
	ret->prev = NULL;
	ret->next = NULL;
	Walle_add_object(ret);

	return ret;
}

Loopr_Value *
Loopr_create_string(Loopr_Byte *data, int *offset)
{
	int length;
	Loopr_Value *ret;

	length = Loopr_mbstowcs_len((char *)data);
	*offset = strlen((char *)data) + 1;

	ret = Loopr_alloc_value(LPR_STRING);

	ret->u.string_value = MEM_malloc(sizeof(Loopr_Char) * (length + 1));
	Loopr_mbstowcs((char *)data, ret->u.string_value);

	return ret;
}

Loopr_Value *
Loopr_create_object(Loopr_Value *orig)
{
	Loopr_Value *ret;

	if (orig && orig->type == LPR_OBJECT) {
		return orig;
	}

	ret = Loopr_alloc_value(LPR_OBJECT);
	ret->u.object_value = orig;

	return ret;
}

Loopr_Value *
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

	return Loopr_create_null();
}
