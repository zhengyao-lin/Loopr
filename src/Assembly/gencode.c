#include <stdio.h>
#include <string.h>
#include "MEM.h"
#include "DBG.h"
#include "SandBox_pri.h"
#include "Assembly.h"

struct ConstTypeMapping_tag {
	ConstantType const_type;
	Edge_BasicType basic_type;
} const_type_mapping[] = {
	{-1,			-1},
	{CONST_CHAR,	EDGE_CHAR},
	{CONST_BYTE,	EDGE_BYTE},
	{CONST_INT16,	EDGE_INT16},
	{CONST_INT32,	EDGE_INT32},
	{CONST_INT64,	EDGE_INT64},

	{CONST_SINGLE,	EDGE_SINGLE},
	{CONST_DOUBLE,	EDGE_DOUBLE},

	{CONST_STRING,	EDGE_STRING},
};

Edge_Byte
Gencode_search_code(char *name)
{
	Edge_Byte i;
	Edge_Byte len = EDGE_CODE_PLUS_1;

	for (i = 1; i < len; i++) {
		if (!strcmp(name, Edge_Byte_Info[i].assembly_name)) {
			return i;
		}
	}
	return -1;
}

Edge_BasicType
Gencode_search_type(char *name)
{
	Edge_BasicType i;
	int len = EDGE_BASIC_TYPE_PLUS_1;

	for (i = 1; i < len; i++) {
		if (!strcmp(name, Edge_Type_Info[i].short_name)) {
			return i;
		}
	}
	return -1;
}

void
Gencode_fix_load_byte(ByteContainer *env, Statement *list)
{
	Edge_BasicType type;

	if (list->bytecode->next != NULL) {
		type = Gencode_search_type(list->bytecode->next->name);
	} else {
		type = const_type_mapping[list->constant->type].basic_type;
	}

	if (!(type > 0 && type <= EDGE_BASIC_TYPE_PLUS_1)) {
		DBG_panic(("line %d: Unknown type argument \"%s\"\n", list->line_number, list->bytecode->next->name));
	}

	Coding_push_code(env, EDGE_LD_BYTE, &type, 1);
	Coding_push_code(env, EDGE_NULL_CODE,
					 Edge_byte_serialize(&list->constant->u.int64_value, Edge_Type_Info[type].size),
					 Edge_Type_Info[type].size);
	return;
}

void
Gencode_push_constant(ByteContainer *env, Constant *constant)
{
	Edge_BasicType type;

	if (constant == NULL) {
		return;
	}
	type = const_type_mapping[constant->type].basic_type;

	switch (constant->type) {
		case CONST_BYTE:
			Coding_push_code(env, EDGE_NULL_CODE,
					 		 &constant->u.byte_value,
					 		 1);
			break;
		case CONST_CHAR:
		case CONST_INT16:
		case CONST_INT32:
		case CONST_INT64:
		case CONST_SINGLE:
		case CONST_DOUBLE:
			Coding_push_code(env, EDGE_NULL_CODE,
					 		 Edge_byte_serialize(&constant->u.int64_value, Edge_Type_Info[type].size),
					 		 Edge_Type_Info[type].size);
			break;
		case CONST_STRING:
			Coding_push_code(env, EDGE_NULL_CODE,
							 MEM_strdup(constant->u.string_value),
							 strlen(constant->u.string_value) + 1);
			break;
		default:
			DBG_panic(("line %d: Unknown constant type %d\n", constant->line_number, constant->type));
			break;
	}
	return;
}

void
Gencode_push_constant_list(ByteContainer *env, Constant *list)
{
	Constant *pos;

	for (pos = list; pos; pos = pos->next) {
		Gencode_push_constant(env, pos);
	}

	return;
}

void
Gencode_push_type_args(ByteContainer *env, Bytecode *code)
{
	Bytecode *pos;
	Edge_BasicType type;

	for (pos = code; pos; pos = pos->next) {
		type = Gencode_search_type(pos->name);
		if (type < EDGE_BASIC_TYPE_PLUS_1 && type > 0) {
			Coding_push_code(env, EDGE_NULL_CODE, &type, 1);
		} else {
			DBG_panic(("line %d: Unknown type argument \"%s\"\n", pos->line_number, pos->name));
		}
	}

	return;
}

void
Gencode_statement(ByteContainer *env, Statement *list)
{
	Edge_Byte code;

	code = Gencode_search_code(list->bytecode->name);
	switch (code) {
		case EDGE_LD_BYTE:
			Gencode_fix_load_byte(env, list);
			break;
		case EDGE_NULL_CODE:
			DBG_panic(("line %d: \"dummy\" is not any useful code\n", list->line_number));
			break;
		case (Edge_Byte)-1:
			DBG_panic(("line %d: Unknown code \"%s\"\n", list->line_number, list->bytecode->name));
			break;
		default:
			Coding_push_code(env, code, NULL, 0);
			Gencode_push_type_args(env, list->bytecode->next);
			Gencode_push_constant_list(env, list->constant);
			break;
	}
	return;
}

void
Gencode_statement_list(ByteContainer *env, StatementList *list)
{
	StatementList *pos;

	for (pos = list; pos; pos = pos->next) {
		Gencode_statement(env, pos->statement);
	}

	return;
}

ByteContainer *
Gencode_compile(Asm_Compiler *compiler)
{
	ByteContainer *container;

	container = Coding_init_coding_env();
	Gencode_statement_list(container, compiler->list);

	return container;
}
