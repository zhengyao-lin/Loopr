#include <string.h>
#include "SandBox_pri.h"
#include "MEM.h"
#include "DBG.h"
#include "UTL.h"

TypeInfo Loopr_Type_Info[] = {
	{"dummy",	"dummy", 		-1},
	{"null",	"Null",			0},

	{"bl",		"Boolean",		sizeof(Loopr_Boolean)},
	{"c4",		"Char",			sizeof(Loopr_Char)},

	{"s8",		"SByte",		sizeof(Loopr_SByte)},
	{"b8",		"Byte",			sizeof(Loopr_Byte)},

	{"i16",		"Int16",		sizeof(Loopr_Int16)},
	{"u16",		"UInt16",		sizeof(Loopr_UInt16)},

	{"i32",		"Int32",		sizeof(Loopr_Int32)},
	{"u32",		"UInt32",		sizeof(Loopr_UInt32)},

	{"i64",		"Int64",		sizeof(Loopr_Int64)},
	{"u64",		"UInt64",		sizeof(Loopr_UInt64)},

	{"f8",		"Single",		sizeof(Loopr_Single)},
	{"f16",		"Double",		sizeof(Loopr_Double)},
	{"str",		"String",		sizeof(Loopr_Char *)},

	{"obj",		"Object",		sizeof(Loopr_Value *)},
};

Loopr_Byte *
Loopr_byte_serialize(const void *data, int length)
{
	Loopr_Byte *ret;

	ret = MEM_malloc(sizeof(Loopr_Byte) * length);
	memcpy(ret, data, length);

	return ret;
}

void*
Loopr_byte_deserialize(void *dest, const Loopr_Byte *data, int length)
{
	return memcpy(dest, data, length);
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
	ret->table = Loopr_alloc_info_table(type);
	MEM_fill(ret->u, NULL_VALUE);

	ret->marked = LPR_False;
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
Loopr_create_null()
{
	return NULL;
}

Loopr_Value *
Loopr_create_object(Loopr_Value *orig)
{
	Loopr_Value *ret;

	if (orig && orig->table->type == LPR_OBJECT) {
		return orig;
	}

	ret = Loopr_alloc_value(LPR_OBJECT);
	ret->u.object_value = orig;

	return ret;
}

Loopr_Value *
Loopr_get_init_value(Loopr_BasicType type)
{
	Loopr_Value *value;

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
	}

	return value;
}
