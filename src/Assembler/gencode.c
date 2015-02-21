#include <stdio.h>
#include <string.h>
#include "LBS.h"
#include "MEM.h"
#include "DBG.h"
#include "Assembler.h"
#include "SandBox_pri.h"

static int nullv = 0x0;
static int not_nullv = LPR_True;
static ByteContainer *top_env = NULL;

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
	{CONST_LABEL,	LPR_STRING},
};

ByteInfo Loopr_CR_Info[] = {
	{"dummy",			0,	0},

	{"entry",			0,	0},
	{"maxstack",		0,	0},
	{"function",		0,	0},
	{"def",				0,	0},
	{"using",			0,	0},
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

static void Gencode_statement(ByteContainer *env, Statement *list);
static void Gencode_statement_list(ByteContainer *env, StatementList *list);

static void
Gencode_push_constant(ByteContainer *env, Constant *constant)
{
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
					 		 (Loopr_Byte *)&constant->u.int64_value,
					 		 Loopr_Type_Info[type].size);
			break;
		case CONST_STRING:
			Coding_push_code(env, LPR_NULL_CODE,
							 (Loopr_Byte *)constant->u.string_value,
							 strlen(constant->u.string_value) + 1);
			break;
		case CONST_LABEL:
			Label_ref(env, constant->u.string_value, env->next);
			Coding_push_code(env, LPR_NULL_CODE,
					 		 (Loopr_Byte *)&nullv,
					 		 sizeof(Loopr_Int32));
			break;
		case CONST_BLOCK: {
			StatementList *pos;
			StatementList *last;

			for (pos = constant->u.block; pos; last = pos, pos = pos->next, ASM_free(last)) {
				Gencode_statement(env, pos->statement);
			}
			break;
		}
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

	for (pos = list; pos; pos = pos->next) {
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

	for (pos = code; pos; pos = pos->next) {
		if (!pos->has_fixed) {
			type = Gencode_search_type(pos->name);
			if (type < LPR_BASIC_TYPE_PLUS_1 && type > 0) {
				Coding_push_code(env, LPR_NULL_CODE, (Loopr_Byte *)&type, 1);
			} else {
				type = Gencode_search_code(pos->name);
				if ((Loopr_Byte)type < LPR_CODE_PLUS_1 && type > 0) {
					Coding_push_code(env, type, NULL, 0);
				} else {
					DBG_panic(("line %d: Unknown type argument \"%s\"\n", pos->line_number, pos->name));
				}
			}
			pos->has_fixed = LPR_True;
		} else {
			type = pos->code;
			Coding_push_code(env, LPR_NULL_CODE, (Loopr_Byte *)&type, 1);
		}
	}

	return type;
}

#define CONST_TYPE_MAP(const_type) \
	(const_type_mapping[(const_type)].basic_type)

static void
Gencode_fix_load_byte(ByteContainer *env, Statement *list)
{
	Loopr_BasicType type;

	Coding_push_code(env, LPR_LD_BYTE, NULL, 0);

	if (list->bytecode->next) {
		type = Gencode_push_type_args(env, list->bytecode->next);
	} else {
		type = CONST_TYPE_MAP(list->constant->type);
		Coding_push_code(env, LPR_NULL_CODE, (Loopr_Byte *)&type, 1);
	}

	if (type <= LPR_INT64) {
		Coding_push_code(env, LPR_NULL_CODE,
						 (Loopr_Byte *)&list->constant->u.int64_value,
						 Loopr_Type_Info[type].size);
	} else { /* float convert */
		if (list->constant->type == CONST_SINGLE) { /* single to double */
			list->constant->u.double_value = (Loopr_Double)list->constant->u.single_value;
		}
		Coding_push_code(env, LPR_NULL_CODE,
					 	 (Loopr_Byte *)&list->constant->u.double_value,
					 	 Loopr_Type_Info[type].size);
	}
	return;
}

static void
Gencode_dispose_bytecode(Bytecode *header)
{
	Bytecode *pos;
	Bytecode *last;

	for (pos = header; pos;
		last = pos, pos = pos->next, ASM_free(last)) {
		if (pos->name) {
			MEM_free(pos->name);
		}
	}
	return;
}

static void
Gencode_dispose_package_name(PackageName *pn)
{
	PackageName *pos;
	PackageName *last;

	for (pos = pn; pos; last = pos, pos = pos->next, ASM_free(last)) {
		if (pos->name) {
			MEM_free(pos->name);
		}
	}

	return;
}

static void
Gencode_dispose_constant(Constant *header)
{
	Constant *pos;
	Constant *last;

	for (pos = header; pos;
		last = pos, pos = pos->next, ASM_free(last)) {
		if (pos->type == CONST_STRING
			|| pos->type == CONST_LABEL) {
			MEM_free(pos->u.string_value);
		} else if (pos->type == CONST_PACKAGE_NAME) {
			Gencode_dispose_package_name(pos->u.package_name_value);
		}
	}
	return;
}

static void
Gencode_function(ByteContainer *env, Constant *arguments)
{
	int i;
	char *name;
	ByteContainer *new_func;
	StatementList *block;
	Constant *pos;

	new_func = Coding_init_coding_env();
	new_func->name = NULL;
	new_func->outer_env = env;

	if (arguments->type == CONST_KEYWORD) {
		switch (arguments->u.keyword_value) {
			case ASM_VOID:
				new_func->is_void = LPR_True;
				arguments = arguments->next;
				break;
			default:
				DBG_panic(("line %d: Unknown keyword %d\n", arguments->line_number, arguments->u.keyword_value));
				break;
		}
	}

	if (!(arguments->type == CONST_STRING || arguments->type == CONST_LABEL)) {
		DBG_panic(("line %d: Nameless function\n", arguments->line_number));
	} else {
		name = arguments->u.string_value;
	}

	arguments = arguments->next;
	for (i = 0, pos = arguments;
		 pos && (pos->type == CONST_STRING || pos->type == CONST_LABEL);
		 pos = pos->next, i++) {
		/*Coding_init_local_variable(new_func, pos->u.string_value);*/
	}
	env->stack_size += i;

	if (!pos) { /* function signed */
		if (!(new_func->native_function = Native_search_function_by_name(name))) {
			DBG_panic(("Failed to find native function by name \"%s\"\n", name));
		}
	} else if (pos->type == CONST_BLOCK) {
		block = pos->u.block;
		Gencode_statement_list(new_func, pos->u.block);
		Coding_push_code(new_func, LPR_RETURN, NULL, 0);
	}

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
Gencode_get_function_index_name_space(ByteContainer *env, char *name, int ns_index)
{
	int i;
	Asm_Compiler *compiler;
	NameSpace *ns;

	compiler = Asm_get_current_compiler();
	ns = &compiler->name_space[ns_index];
	for (i = 0; i < ns->function_count; i++) {
		if (!strcmp(ns->function_definition[i].name, name)) {
			return i;
		}
	}

	/* DBG_panic(("Undefined function \"%s\"\n", name)); */

	return -1;
}

static int
Gencode_get_function_index(ByteContainer *env, char *name)
{
	return Gencode_get_function_index_name_space(env, name, Asm_get_current_compiler()->current_name_space_index);
}

static int
Gencode_search_name_space_index(char *identifier)
{
	int i;
	Asm_Compiler *current_compiler;

	if (identifier == NULL) {
		return 0;
	}

	current_compiler = Asm_get_current_compiler();
	for (i = 0; i < current_compiler->name_space_count; i++) {
		if (!strcmp(current_compiler->name_space[i].name, identifier)) {
			return i;
		}
	}

	DBG_panic(("Cannot find namespace by identifier \"%s\"\n", identifier));

	return -1;
}

static void
Gencode_add_using(ByteContainer *env, char *name)
{
	UsingList *new_using;
	UsingList *pos;

	for (pos = env->using_list; pos && pos->next; pos = pos->next);

	new_using = ASM_malloc(sizeof(UsingList));
	new_using->name_space = Gencode_search_name_space_index(name);
	new_using->next = NULL;

	if (pos) {
		pos->next = new_using;
	} else {
		env->using_list = new_using;
	}

	return;
}

static void
Gencode_compiler_reference(ByteContainer *env, Statement *list)
{
	int index;
	Loopr_Byte code;

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
			break;
		case LCR_FUNCTION:
			Gencode_function(env, list->constant);
			break;
		case LCR_DEFINE:
			if (list->constant->type != CONST_STRING
				&& list->constant->type != CONST_LABEL) {
				DBG_panic(("line %d: a variable's name can only be a label or string\n", list->line_number));
			}
			index = Coding_init_local_variable(env, list->constant->u.string_value);
			break;
		case LCR_USING:
			if (list->constant->type != CONST_STRING
				&& list->constant->type != CONST_LABEL) {
				DBG_panic(("line %d: using name can only be a label or string\n", list->line_number));
			}
			Gencode_add_using(env, list->constant->u.string_value);
			break;
		default:
			DBG_panic(("line %d: Unknown CR code \"%s\"\n", list->line_number,
															list->bytecode->next->name));
			break;
	}

	Gencode_dispose_bytecode(list->bytecode);
	Gencode_dispose_constant(list->constant);
	ASM_free(list);
}

static Loopr_Boolean
Gencode_is_void(int ns, int index)
{
	return Asm_get_current_compiler()->name_space[ns].function_definition[index].is_void;
}

static void
Gencode_fix_function_invoke(ByteContainer *env, PackageName *pkgn, char *name, int argc)
{
	Asm_Compiler *compiler = Asm_get_current_compiler();
	UsingList *pos;
	int name_space = compiler->current_name_space_index;
	int index;

	if (pkgn) {
		name_space = Gencode_search_name_space_index(pkgn->name);
	}

	index = Gencode_get_function_index_name_space(env, name, name_space);
	if (index < 0) {
		for (pos = top_env->sub_name_space[compiler->current_name_space_index]->using_list;
			 pos; pos = pos->next) {
			if ((index = Gencode_get_function_index_name_space(env, name, pos->name_space)) >= 0) {
				name_space = pos->name_space;
				break;
			}
		}
		for (pos = env->using_list; pos; pos = pos->next) {
			if ((index = Gencode_get_function_index_name_space(env, name, pos->name_space)) >= 0) {
				name_space = pos->name_space;
				break;
			}
		}
		if (index < 0) {
			DBG_panic(("Undefined function \"%s\"\n", name));
			return;
		}
	}

	Coding_push_code(env, LPR_INVOKE, NULL, 0);
	Coding_push_code(env, LPR_NULL_CODE,
					 &name_space,
					 sizeof(Loopr_Int32));
	Coding_push_code(env, LPR_NULL_CODE,
					 (Loopr_Byte *)&index,
					 sizeof(Loopr_Int32));
	Coding_push_one_byte(env, argc);
	if (Gencode_is_void(name_space, index)) {
		Coding_push_code(env, LPR_POP, NULL, 0);
	}

	return;
}

static void
Gencode_statement(ByteContainer *env, Statement *list)
{
	int index;
	int argc;
	Loopr_BasicType type;
	Loopr_Byte code;
	Asm_Compiler *compiler;
	NameSpace *ns;
	Constant *cpos;

	compiler = Asm_get_current_compiler();
	ns = &compiler->name_space[compiler->current_name_space_index];

	if (list->label) {
		Label_add(env, list->label, env->next);
		Gencode_push_constant_list(env, list->constant);
		MEM_free(list->label);
		goto DISPOSE;
	}

	if (list->bytecode->name) {
		code = Gencode_search_code(list->bytecode->name);
	} else if (list->bytecode->has_fixed) {
		code = list->bytecode->code;
	} else {
		if (list->bytecode->next) {
			Gencode_compiler_reference(env, list);
			return;
		} else {
			DBG_panic(("line %d: empty compiler reference code\n", list->line_number));
		}
	}

	switch (code) {
		case LPR_LD_BYTE:
			Gencode_fix_load_byte(env, list);
			break;
		case LPR_LD_LOC:
		case LPR_STORE_LOC:
			if (list->constant->type != CONST_STRING
				&& list->constant->type != CONST_LABEL) {
				DBG_panic(("line %d: a variable's name can only be a label or string\n", list->line_number));
			}
			index = Coding_get_local_variable_index(env, list->constant->u.string_value);

			Coding_push_code(env, code, NULL, 0);
			Gencode_push_type_args(env, list->bytecode->next);
			Coding_push_code(env, LPR_NULL_CODE,
							 (Loopr_Byte *)&index,
							 sizeof(Loopr_Int32));
			break;
		case LPR_INVOKE:
			if (list->constant->type == CONST_PACKAGE_NAME) {
				Gencode_fix_function_invoke(env, list->constant->u.package_name_value,
											Gencode_get_constant_by_index(list->constant, 1)->u.string_value,
											Gencode_get_constant_by_index(list->constant, 2)->u.int32_value);
			} else {
				Gencode_fix_function_invoke(env, NULL,
											Gencode_get_constant_by_index(list->constant, 0)->u.string_value,
											Gencode_get_constant_by_index(list->constant, 1)->u.int32_value);
			}
			break;
		case LPR_BRANCH:
			Coding_push_code(env, code, NULL, 0);
			Coding_push_one_byte(env, LPR_True);

			if (list->bytecode->next != NULL) {
				Gencode_push_type_args(env, list->bytecode->next);
			} else {
				DBG_panic(("line %d: no argument for code \"br\" ( suppose to be true or false )\n", list->line_number));
			}

			if (list->constant != NULL && list->constant->type == CONST_BLOCK) {
				index = env->next;

				env->code[env->next-2] = LPR_False;

				Coding_push_code(env, LPR_NULL_CODE,
								 (Loopr_Byte *)&nullv,
								 sizeof(Loopr_Int32));
				Gencode_push_constant_list(env, list->constant);
				memcpy(&env->code[index],
					   &env->next, sizeof(Loopr_Int32));
			} else {
				Gencode_push_constant_list(env, list->constant);
			}
			break;
		case LPR_NEW_ARRAY:
			type = LPR_INT32;
			for (cpos = list->constant, index = 0; cpos; cpos = cpos->next, index++) {
				Coding_push_code(env, LPR_LD_BYTE, &type, 1);
				Coding_push_code(env, LPR_NULL_CODE,
						 		(Loopr_Byte *)&cpos->u.int32_value,
						 		Loopr_Type_Info[type].size);
			}

			if (index) {
				Coding_push_code(env, code, &index, 1);
			} else {
				Coding_push_code(env, code, NULL, 0);
				Gencode_push_type_args(env, list->bytecode->next);
				Gencode_push_constant_list(env, list->constant);
			}
			break;
		case LPR_LD_ARRAY:
			for (cpos = list->constant; cpos; cpos = cpos->next) {
				Coding_push_code(env, code,
								(Loopr_Byte *)&cpos->u.int32_value,
								sizeof(Loopr_Int32));
			}
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

DISPOSE:
	Gencode_dispose_bytecode(list->bytecode);
	Gencode_dispose_constant(list->constant);
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
	Label_set_all(env);

	return;
}

ByteContainer *
Gencode_compile(Asm_Compiler *compiler)
{
	int i;
	int default_index;
	ByteContainer *container;

	Asm_set_current_compiler(compiler);
	container = Coding_init_coding_env();
	top_env = container;

	default_index = Gencode_search_name_space_index(compiler->default_name_space);

	container->self_reflect = default_index;
	container->sub_name_space_count = compiler->name_space_count;
	container->sub_name_space = MEM_malloc(sizeof(ByteContainer) * compiler->name_space_count);
	for (i = 0; i < compiler->name_space_count; i++) {
		if (i != default_index) {
			compiler->current_name_space_index = i;
			container->sub_name_space[i] = Coding_init_coding_env();
			Gencode_statement_list(container->sub_name_space[i],
								   compiler->name_space[i].top_level);
		} else {
			container->sub_name_space[i] = container;
		}
	}

	compiler->current_name_space_index = default_index;
	Gencode_statement_list(container, compiler->name_space[default_index].top_level);

	Asm_clean_local_env(container);

	return container;
}
