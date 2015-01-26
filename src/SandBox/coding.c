#include <string.h>
#include "SandBox_pri.h"
#include "MEM.h"

ByteInfo Edge_Byte_Info[] = {
	{"dummy",	0,	0},
	{"ldb",		0,	1},
	{"ldnull",	0,	1},
	{"ldstr",	0,	1},
	{"ldloc",	0,	1},
	{"init",	0,	0},

	{"popb",	1,	-1},
	{"popf",	1,	-1},
	{"popstr",	1,	-1},

	{"stloc",	1,	-1},
	{"addb",	2,	-1},
	{"addf",	2,	-1},
	{"addstr",	2,	-1},
	{"subb",	2,	-1},
	{"mulb",	2,	-1},
	{"divb",	2,	-1},

	{"jmp",		0,	0},
	{"exit",	0,	0},
};

Edge_Byte *
Coding_alloc_byte(int length)
{
	Edge_Byte *ret = MEM_malloc(sizeof(Edge_Byte) * length);
	return ret;
}

ByteContainer *
Coding_init_coding_env(void)
{
	ByteContainer *env;

	env = MEM_malloc(sizeof(ByteContainer));
	env->next = 0;
	env->alloc_size = 0;
	env->stack_size = 0;
	env->code = NULL;

	return env;
}

void
Coding_byte_cat(ByteContainer *env, Edge_Byte *src, int count)
{
	env->alloc_size += count;

	env->code = MEM_realloc(env->code,
							sizeof(Edge_Byte) * env->alloc_size);

	memcpy(&env->code[env->next], src, count);
	env->next += count;
}

#define check_is_code(c) ((c) > 0 && (c) < EDGE_CODE_PLUS_1 ? (c) : 0)
#define check_is_negative(num) ((num) < 0 ? 0 : (num))

void
Coding_push_code(ByteContainer *env, Edge_Byte code, Edge_Byte *args, int args_count)
{
	if (code != EDGE_NULL_CODE) {
		Coding_byte_cat(env, &code, 1);
		env->stack_size += check_is_negative(Edge_Byte_Info[check_is_code(code)].stack_regulator);
	}
	Coding_byte_cat(env, args, args_count);
}

ExeEnvironment *
Coding_init_exe_env(ByteContainer *env, WarningFlag wflag)
{
	ExeEnvironment *ret;
	Edge_Value **stack_value;

	ret = MEM_malloc(sizeof(ExeEnvironment));
	ret->wflag = wflag;
	ret->entrance = 0;
	ret->code_length = env->alloc_size;
	ret->code = env->code;
	stack_value = MEM_malloc(sizeof(Edge_Value *) * (env->stack_size + 1));

	ret->stack.alloc_size = env->stack_size + 1;
	ret->stack.stack_pointer = -1;
	ret->stack.value = stack_value;
	ret->local_variable_count = 0;
	ret->local_variable = NULL;

	ret->outer_env = NULL;

	return ret;
}
