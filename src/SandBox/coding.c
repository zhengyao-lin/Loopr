#include <string.h>
#include "LBS.h"
#include "MEM.h"
#include "DBG.h"
#include "SandBox_pri.h"

ByteInfo Loopr_Byte_Info[] = {
	{"dummy",	0,	0},

	{"ldb",		0,	1},
	{"ldc",		0,	1},
	{"ldnull",	0,	1},
	{"ldstr",	0,	1},
	{"ldloc",	0,	1},
	{"ldarg",	0,	1},
	{"ldarr",	1,	0},

	{"conv",	1,	0},
	{"box",		1,	0},
	{"unbox",	1,	0},

	{"pop",		1,	-1},
	{"popb",	1,	-1},
	{"popf",	1,	-1},
	{"popstr",	1,	-1},

	{"stloc",	1,	-1},
	{"starr",	2,	-2},

	{"eqf",		2,	-1},
	{"eq",		2,	-1},
	{"nef",		2,	-1},
	{"ne",		2,	-1},
	{"gtf",		2,	-1},
	{"gt",		2,	-1},
	{"ltf",		2,	-1},
	{"lt",		2,	-1},
	{"lef",		2,	-1},
	{"le",		2,	-1},
	{"gef",		2,	-1},
	{"ge",		2,	-1},

	{"br",		1,	-1},
	{"dup",		1,	1},

	{"add",		2,	-1},
	{"addf",	2,	-1},
	{"addstr",	2,	-1},
	{"subf",	2,	-1},
	{"sub",		2,	-1},
	{"mulf",	2,	-1},
	{"mul",		2,	-1},
	{"divf",	2,	-1},
	{"div",		2,	-1},
	{"modf",	2,	-1},
	{"mod",		2,	-1},
	{"incf",	1,	0},
	{"inc",		1,	0},
	{"decf",	1,	0},
	{"dec",		1,	0},
	{"minf",	1,	0},
	{"min",		1,	0},

	{"invoke",	0,	0},
	{"newarr",	1,	0},

	{"goto",	0,	0},
	{"ret",		0,	0},
	{"nop",		0,	0}
};

Loopr_Byte *
Coding_alloc_byte(int length)
{
	Loopr_Byte *ret = MEM_malloc(sizeof(Loopr_Byte) * length);

	return ret;
}

ByteContainer *
Coding_init_coding_env(void)
{
	ByteContainer *env;

	env = MEM_malloc(sizeof(ByteContainer));
	env->name = NULL;
	env->is_void = LPR_False;

	env->label_header = NULL;
	env->using_list = NULL;

	env->next = 0;
	env->alloc_size = 0;
	env->hinted = LPR_False;
	env->stack_size = 0;
	env->entrance = 0;
	env->code = NULL;

	env->local_variable_count = 0;
	env->local_variable = NULL;

	env->sub_name_space_count = 0;
	env->sub_name_space = NULL;

	env->function_count = 0;
	env->function = NULL;

	env->native_function = NULL;

	env->outer_env = NULL;

	return env;
}

int
Coding_init_local_variable(ByteContainer *env, char *identifier)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (env->local_variable[i].identifier
			&& !strcmp(env->local_variable[i].identifier,
					   identifier)) {
			DBG_panic(("Duplicated variable name \"%s\"\n", identifier));
			return -1;
		}
	}

	env->local_variable = MEM_realloc(env->local_variable,
									  sizeof(LocalVariable) * (env->local_variable_count + 1));
	env->local_variable[env->local_variable_count].value = NULL_REF;
	env->local_variable[env->local_variable_count].identifier = identifier ? MEM_strdup(identifier) : NULL;
	env->local_variable_count++;

	return i;
}

int
Coding_get_local_variable_index(ByteContainer *env, char *name)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (env->local_variable[i].identifier
			&& !strcmp(env->local_variable[i].identifier,
					   name)) {
			return i;
		}
	}
	DBG_panic(("Cannot find local variable \"%s\"\n", name));

	return -1;
}

void
Coding_byte_cat(ByteContainer *env, Loopr_Byte *src, int count)
{
	env->alloc_size += count;

	env->code = MEM_realloc(env->code,
							sizeof(Loopr_Byte) * env->alloc_size);

	memcpy(&env->code[env->next], src, count);
	env->next += count;

	return;
}

#define check_is_code(c) ((c) > 0 \
						  && (c) < LPR_CODE_PLUS_1 \
						  && (c) != LPR_NULL_CODE ? (c) : LPR_False)
#define check_is_negative(num) ((num) < 0 ? 0 : (num))

void
Coding_push_code(ByteContainer *env, Loopr_Byte code, Loopr_Byte *args, int args_count)
{
	if (check_is_code(code)) {
		Coding_byte_cat(env, &code, 1);
		if (!env->hinted) {
			env->stack_size += check_is_negative(Loopr_Byte_Info[code].stack_regulator);
		}
	}
	Coding_byte_cat(env, args, args_count);

	return;
}

void
Coding_push_one_byte(ByteContainer *env, Loopr_Byte code)
{
	Coding_byte_cat(env, &code, 1);
	return;
}

ExeContainer *
Coding_alloc_exe_container(ByteContainer *env)
{
	ExeContainer *ret;

	ret = MEM_malloc(sizeof(ExeContainer));
	ret->entrance = ret->entrance = env->entrance;
	ret->length = env->alloc_size;
	ret->code = env->code;

	return ret;
}

ExeEnvironment *
Coding_init_exe_env(ByteContainer *env, WarningFlag wflag)
{
	int i;
	ExeEnvironment *ret;

	if (!env) {
		return NULL;
	}

	ret = MEM_malloc(sizeof(ExeEnvironment));
	ret->wflag = wflag;
	ret->exe = Coding_alloc_exe_container(env);
	if (env->stack_size < 0) {
		DBG_panic(("negative stack size\n"));
	}

	ret->stack.alloc_size = env->stack_size + 2; /* the last added two is for callinfo and overflow check */
	ret->stack.stack_pointer = -1;
	ret->stack.value = NULL;
	ret->stack.ref_flag = NULL;
	ret->local_variable_map = MEM_malloc(sizeof(LocalVariableMap));
	ret->local_variable_map->count = env->local_variable_count;
	ret->local_variable_map->variable = NULL;
	ret->local_variable_map->prev = NULL;

	ret->self_reflect = env->self_reflect;
	ret->sub_name_space_count = env->sub_name_space_count;
	ret->sub_name_space = NULL;
	if (env->sub_name_space_count > 0) {
		ret->sub_name_space = MEM_malloc(sizeof(ExeEnvironment *) * ret->sub_name_space_count);
		for (i = 0; i < env->sub_name_space_count; i++) {
			if (env->sub_name_space[i] != env) {
				ret->sub_name_space[i] = Coding_init_exe_env(env->sub_name_space[i], wflag);
			} else {
				ret->sub_name_space[i] = ret;
			}
		}
	}

	ret->function_count = env->function_count;
	ret->function = NULL;
	if (env->function_count > 0) {
		ret->function = MEM_malloc(sizeof(ExeEnvironment *) * ret->function_count);
		for (i = 0; i < env->function_count; i++) {
			ret->function[i] = Coding_init_exe_env(env->function[i], wflag);
		}
	}
	ret->native_function = env->native_function;

	return ret;
}
