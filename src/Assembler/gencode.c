#include <stdio.h>
#include <string.h>
#include "MEM.h"
#include "DBG.h"
#include "SandBox_pri.h"
#include "Assembler.h"

struct ConstTypeMapping_tag {
	ConstantType const_type;
	Loopr_BasicType basic_type;
} const_type_mapping[] = {
	{-1,			-1},
	{CONST_CHAR,	LPR_CHAR},
	{CONST_BYTE,	LPR_BYTE},
	{CONST_INT16,	LPR_INT16},
	{CONST_UINT16,	LPR_UINT16},

	{CONST_INT32,	LPR_INT32},
	{CONST_UINT32,	LPR_UINT32},

	{CONST_INT64,	LPR_INT64},
	{CONST_UINT64,	LPR_UINT64},

	{CONST_SINGLE,	LPR_SINGLE},
	{CONST_DOUBLE,	LPR_DOUBLE},

	{CONST_STRING,	LPR_STRING},
};

ByteInfo Loopr_CR_Info[] = {
	{"dummy",			0,	0},

	{"entry",			0,	0},
	{"maxstack",		0,	0},
	{"function",		0,	0},
};

static Loopr_Byte
Gencode_search_code(char *name)
{
	Loopr_Byte i;
	Loopr_Byte len = LPR_CODE_PLUS_1;

	for (i = 1; i < len; i++) {
		if (!strcmp(name, Loopr_Byte_Info[i].assembly_name)) {
			return i;
		}
	}

	return -1;
}

static Loopr_Byte
Gencode_search_CR(char *name)
{
	Loopr_Byte i;
	Loopr_Byte len = LCR_CODE_PLUS_1;

	for (i = 1; i < len; i++) {
		if (!strcmp(name, Loopr_CR_Info[i].assembly_name)) {
			return i;
		}
	}

	return -1;
}

static Loopr_BasicType
Gencode_search_type(char *name)
{
	Loopr_BasicType i;
	int len = LPR_BASIC_TYPE_PLUS_1;

	for (i = 1; i < len; i++) {
		if (!strcmp(name, Loopr_Type_Info[i].short_name)) {
			return i;
		}
	}

	return -1;
}

static void
Gencode_push_constant(ByteContainer *env, Constant *constant)
{
	int nullv = 0x0;
	Loopr_BasicType type;

	if (constant == NULL) {
		return;
	}
	type = const_type_mapping[constant->type].basic_type;

	switch (constant->type) {
		case CONST_BYTE:
			Coding_push_code(env, LPR_NULL_CODE,
					 		 &constant->u.byte_value,
					 		 1);
			break;
		case CONST_CHAR:
		case CONST_INT16:
		case CONST_UINT16:
		case CONST_INT32:
		case CONST_UINT32:
		case CONST_INT64:
		case CONST_UINT64:
		case CONST_SINGLE:
		case CONST_DOUBLE:
			Coding_push_code(env, LPR_NULL_CODE,
					 		 &constant->u.int64_value,
					 		 Loopr_Type_Info[type].size);
			break;
		case CONST_STRING:
			Coding_push_code(env, LPR_NULL_CODE,
							 constant->u.string_value,
							 strlen(constant->u.string_value) + 1);
			MEM_free(constant->u.string_value);
			break;	
		case CONST_LABEL:
			Label_ref(constant->u.string_value, env->next);
			Coding_push_code(env, LPR_NULL_CODE,
					 		 &nullv,
					 		 sizeof(Loopr_Int32));
			MEM_free(constant->u.string_value);
			break;
		default:
			DBG_panic(("line %d: Unknown constant type %d\n", constant->line_number, constant->type));
			break;
	}

	return;
}

static void
Gencode_push_constant_list(ByteContainer *env, Constant *list)
{
	Constant *pos;
	Constant *last;

	for (pos = list; pos; last = pos, pos = pos->next, ASM_free(last)) {
		Gencode_push_constant(env, pos);
	}

	return;
}

static int
Gencode_push_type_args(ByteContainer *env, Bytecode *code)
{
	Bytecode *pos;
	Bytecode *last;
	Loopr_BasicType type = -1;

	for (pos = code; pos; last = pos, pos = pos->next, ASM_free(last)) {
		if (!pos->has_fixed) {
			type = Gencode_search_type(pos->name);
			if (type < LPR_BASIC_TYPE_PLUS_1 && type > 0) {
				Coding_push_code(env, LPR_NULL_CODE, &type, 1);
			} else {
				DBG_panic(("line %d: Unknown type argument \"%s\"\n", pos->line_number, pos->name));
			}
			MEM_free(pos->name);
			pos->has_fixed = LPR_True;
		} else {
			type = pos->code;
			Coding_push_code(env, LPR_NULL_CODE, &type, 1);
		}
	}

	return type;
}

static void
Gencode_fix_load_byte(ByteContainer *env, Statement *list)
{
	Constant *pos;
	Loopr_BasicType type;

	Coding_push_code(env, LPR_LD_BYTE, NULL, 0);

	if (list->bytecode->next) {
		type = Gencode_push_type_args(env, list->bytecode->next);
	} else {
		type = const_type_mapping[list->constant->type].basic_type;
		Coding_push_code(env, LPR_NULL_CODE, &type, 1);
	}

	Coding_push_code(env, LPR_NULL_CODE,
					 &list->constant->u.int64_value,
					 Loopr_Type_Info[type].size);
	ASM_free(list->constant);
	return;
}

static void Gencode_statement(ByteContainer *env, Statement *list);
static void Gencode_statement_list(ByteContainer *env, StatementList *list);

static void
Gencode_function(ByteContainer *env, char *name, int argc, Statement *block)
{
	int i;
	ByteContainer *new_func;

	new_func = Coding_init_coding_env();
	new_func->name = name;
	for (i = 0; i < argc; i++)
	{
		Coding_init_local_variable(new_func, NULL);
	}
	new_func->outer_env = env;

	Gencode_statement_list(new_func, block);
	Asm_clean_local_env(new_func);

	env->function = MEM_realloc(env->function, sizeof(ByteContainer *) * (env->function_count + 1));
	env->function[env->function_count] = new_func;
	env->function_count++;

	return;
}

static Constant *
Gencode_get_constant_by_index(Constant *head, int index)
{
	Constant *pos;
	for (pos = head; pos && index > 0; pos = pos->next, index--)
		;

	if (index > 0) {
		return NULL;
	} else {
		return pos;
	}
}

static int
Gencode_get_function_index(ByteContainer *env, char *name)
{
	int i;
	Asm_Compiler *compiler;

	compiler = Asm_get_current_compiler();
	for (i = 0; i < compiler->function_count; i++) {
		if (!strcmp(compiler->function_definition[i].name, name)) {
			return i;
		}
	}

	DBG_panic(("Undefined function \"%s\"\n", name));

	return -1;
}

static void
Gencode_statement(ByteContainer *env, Statement *list)
{
	int index;
	Loopr_Byte code;

	if (list->label) {
		Label_add(list->label, env->next);
		MEM_free(list->label);
		ASM_free(list);
		return;
	}

	if (list->bytecode->name) {
		code = Gencode_search_code(list->bytecode->name);
	} else if (list->bytecode->has_fixed) {
		code = list->bytecode->code;
	} else {
		if (list->bytecode->next) {
			code = Gencode_search_CR(list->bytecode->next->name);
			switch (code) {
				case LCR_ENTRANCE:
					env->entrance = env->next;
					break;
				case LCR_MAX_STACK:
					env->hinted = LPR_True;
					if (list->constant) {
						env->stack_size = list->constant->u.int32_value;
					} else {
						DBG_panic(("line %d: in CR code \"%s\": too few arguments\n",
								   list->line_number,
								   Loopr_CR_Info[LCR_MAX_STACK].assembly_name));
					}
					ASM_free(list->constant);
					break;
				case LCR_FUNCTION:
					Gencode_function(env, list->constant->u.string_value, list->constant->next->u.int32_value,
									 Gencode_get_constant_by_index(list->constant, 2)->u.block);
					ASM_free(list->constant);
					ASM_free(list->constant->next);
					ASM_free(list->constant->next->next);
					break;
				default:
					DBG_panic(("line %d: Unknown CR code \"%s\"\n", list->line_number,
																	list->bytecode->next->name));
					break;
			}
			MEM_free(list->bytecode->next->name);
			ASM_free(list->bytecode->next);
			ASM_free(list->bytecode);
			ASM_free(list);
			return;
		} else {
			DBG_panic(("line %d: empty compiler reference code\n", list->line_number));
		}
	}

	switch (code) {
		case LPR_LD_BYTE:
			Gencode_fix_load_byte(env, list);
			break;
		case LPR_INIT_LOC:
		case LPR_LD_LOC:
		case LPR_STORE_LOC:
			if (list->constant->type != CONST_STRING
				&& list->constant->type != CONST_LABEL) {
				DBG_panic(("line %d: a variable's name can only be a label or string\n", list->line_number));
			} else if (code == LPR_INIT_LOC) {
				index = Coding_init_local_variable(env, list->constant->u.string_value);
			} else {
				index = Coding_get_local_variable_index(env, list->constant->u.string_value);
			}
			Coding_push_code(env, code, NULL, 0);
			Gencode_push_type_args(env, list->bytecode->next);
			Coding_push_code(env, LPR_NULL_CODE,
							 &index,
							 sizeof(Loopr_Int32));
			MEM_free(list->constant->u.string_value);
			ASM_free(list->constant);
			break;
		case LPR_CALL:
			Coding_push_code(env, code, NULL, 0);
			index = Gencode_get_function_index(env, list->constant->u.string_value);
			Coding_push_code(env, LPR_NULL_CODE,
							 &index,
							 sizeof(Loopr_Int32));
			Coding_push_code(env, LPR_NULL_CODE,
							 &list->constant->next->u.int32_value,
							 sizeof(Loopr_Int32));
			MEM_free(list->constant->u.string_value);
			ASM_free(list->constant);
			ASM_free(list->constant->next);
			break;
		case LPR_NULL_CODE:
			DBG_panic(("line %d: \"dummy\" is not any useful code\n", list->line_number));
			break;
		case (Loopr_Byte)-1:
			DBG_panic(("line %d: Unknown code \"%s\"\n", list->line_number, list->bytecode->name));
			break;
		default:
			Coding_push_code(env, code, NULL, 0);
			Gencode_push_type_args(env, list->bytecode->next);
			Gencode_push_constant_list(env, list->constant);
			break;
	}
	MEM_free(list->bytecode->name);
	ASM_free(list->bytecode);
	ASM_free(list);

	return;
}

static void
Gencode_statement_list(ByteContainer *env, StatementList *list)
{
	StatementList *pos;
	StatementList *last;

	Label_init(env);
	for (pos = list; pos; last = pos, pos = pos->next, ASM_free(last)) {
		Gencode_statement(env, pos->statement);
	}
	Label_set_all();

	return;
}

ByteContainer *
Gencode_compile(Asm_Compiler *compiler)
{
	ByteContainer *container;

	container = Coding_init_coding_env();
	Gencode_statement_list(container, compiler->top_level);
	Asm_clean_local_env(container);

	return container;
}
