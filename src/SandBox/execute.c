#include <stdio.h>
#include <string.h>
#include "DBG.h"
#include "UTL.h"
#include "MEM.h"
#include "SandBox_pri.h"

#define ST(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)])
#define ST_i(stack, p) \
	((stack).value[p])

#define ST_TYPE(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->table->type)

#define ST_BYTE(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.byte_value)
#define ST_SBYTE(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.sbyte_value)
#define ST_INT16(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.int16_value)
#define ST_UINT16(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.uint16_value)
#define ST_INT32(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.int32_value)
#define ST_UINT32(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.uint32_value)
#define ST_STRING(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.string_value)
#define ST_INT64(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.int64_value)
#define ST_UINT64(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.uint64_value)
#define ST_SINGLE(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.single_value)
#define ST_DOUBLE(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.double_value)
#define ST_FLOAT(stack, offset) \
	(ST_TYPE((stack), (offset)) == EDGE_SINGLE \
	? (stack).value[(stack).stack_pointer + (offset)]->u.single_value \
	: (stack).value[(stack).stack_pointer + (offset)]->u.double_value)
#define ST_STRING(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.string_value)

#define ST_WRITE(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)] = r)

#define ST_WRITE_BYTE(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.byte_value = r)
#define ST_WRITE_SBYTE(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.sbyte_value = r)
#define ST_WRITE_INT16(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.int16_value = r)
#define ST_WRITE_UINT16(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.uint16_value = r)
#define ST_WRITE_INT32(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.int32_value = r)
#define ST_WRITE_UINT32(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.uint32_value = r)
#define ST_WRITE_INT64(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.int64_value = r)
#define ST_WRITE_UINT64(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.uint64_value = r)
#define ST_WRITE_SINGLE(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.single_value = r)
#define ST_WRITE_DOUBLE(stack, offset, r) \
	((stack).value[(stack).stack_pointer + (offset)]->u.double_value = r)

#define TYPE_CMP(t1, t2) \
	((t1) >= (t2) ? (t1) : (t2))
#define GET_BIT(num, type) \
	((num) << (Edge_Type_Info[EDGE_INT64].size - Edge_Type_Info[type].size) * 8 \
		   >> (Edge_Type_Info[EDGE_INT64].size - Edge_Type_Info[type].size) * 8)

#define get_visual_string(src) (src ? src : NULL_VISUAL)

Edge_Char *
chain_string(Edge_Value *str1, Edge_Value *str2)
{
	int len;
	Edge_Char *ret;
	Edge_Char *right;
	Edge_Char *left;

	right = get_visual_string(str1->u.string_value);
	left = get_visual_string(str2->u.string_value);

	len = Edge_wcslen(right) + Edge_wcslen(left);
	ret = MEM_malloc(sizeof(Edge_Char) * (len + 1));

	Edge_wcscpy(ret, right);
	Edge_wcscat(ret, left);

	if (str1->u.string_value) {
		MEM_free(str1->u.string_value);
	}

	return ret;
}

Edge_Value *
Private_do_push_byte(Edge_BasicType basic_type, Edge_Byte *code, int *offset)
{
	Edge_Value *ret;

	ret = Edge_alloc_value(basic_type);

	MEM_fill(ret->u, NULL_VALUE);
	Edge_byte_deserialize(&ret->u.int64_value,
						  code, Edge_Type_Info[basic_type].size);
	*offset = Edge_Type_Info[basic_type].size;

	return ret;
}

void
Private_init_local_variable(ExeEnvironment *env, Edge_BasicType type, char *identifier)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (!strcmp(env->local_variable[i].identifier,
					identifier)) {
			if (env->wflag != EDGE_NOTHING) {
				DBG_panic(("Duplicated variable name \"%s\"\n", identifier));
			}
			return;
		}
	}

	env->local_variable = MEM_realloc(env->local_variable,
									  sizeof(LocalVariable) * (env->local_variable_count + 1));
	env->local_variable[env->local_variable_count].value = Edge_get_init_value(type);
	env->local_variable[env->local_variable_count].identifier = MEM_strdup(identifier);

	env->local_variable_count++;
}

Edge_Value *
Private_copy_variable(Edge_Value *src)
{
	Edge_Value *ret;

	ret = Edge_alloc_value(src->table->type);
	switch (src->table->type) {
		case EDGE_STRING:
			if (src->u.string_value) {
				ret->u.string_value = MEM_malloc(sizeof(Edge_Char) * (Edge_wcslen(src->u.string_value) + 1));
				Edge_wcscpy(ret->u.string_value, src->u.string_value);
			} else {
				ret->u.string_value = NULL;
			}
			break;
	}

	return ret;
}

void
Private_assign_local_variable(ExeEnvironment *env, char *name, Edge_Value *value)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (!strcmp(env->local_variable[i].identifier,
					name)) {
			/*if (env->local_variable[i].value) {
				Private_dispose_value(&env->local_variable[i].value);
			}*/
			env->local_variable[i].value = Private_copy_variable(value);
			return;
		}
	}
	if (env->wflag != EDGE_NOTHING) {
		DBG_panic(("Cannot find local variable \"%s\"\n", name));
	}
}

Edge_Value *
Private_load_local_variable(ExeEnvironment *env, char *name)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (!strcmp(env->local_variable[i].identifier,
					name)) {
			return Private_copy_variable(env->local_variable[i].value);
		}
	}
	if (env->wflag != EDGE_NOTHING) {
		DBG_panic(("Cannot find local variable \"%s\"\n", name));
	}
}

void
Private_dispose_value(Edge_Value **target)
{
	switch ((*target)->table->type) {
		case EDGE_STRING:
			MEM_free((*target)->u.string_value);
			break;
	}
	MEM_free((*target)->table);
	MEM_free(*target);
	*target = NULL;
}

void
Private_expand_stack(Edge_Stack *orig, int resize)
{
	orig->value = MEM_realloc(orig->value, sizeof(Edge_Value *) * resize);
	orig->alloc_size = resize;
	return;
}

void
Private_walle_marker(ExeEnvironment *env)
{
	int i;

	for (i = 0; i < env->local_variable_count; i++) {
		env->local_variable[i].value->marked = True;
	}

	for (i = 0; i <= env->stack.stack_pointer; i++) {
		env->stack.value[i]->marked = True;
	}

	return;
}

void
Edge_execute(ExeEnvironment *env)
{
	int prev_stack;
	int pc = env->entrance;

	for (; pc < env->code_length;) {

		printf("%4d:\t%-15ssp(%d)\n", pc, Edge_Byte_Info[env->code[pc]].assembly_name, env->stack.stack_pointer);
		prev_stack = env->stack.stack_pointer;

		if (env->stack.stack_pointer < (Edge_Byte_Info[env->code[pc]].need_stack - 1)
			&& env->wflag != EDGE_NOTHING) {
			DBG_panic(("Too few stack for code \"%s\"\n", Edge_Byte_Info[env->code[pc]].assembly_name));
		}

		if (env->stack.stack_pointer >= env->stack.alloc_size - 1) {
			if (env->wflag > EDGE_JUST_PANIC) {
				DBG_panic(("Stack overflow\n"));
			} else {
				Private_expand_stack(&env->stack, env->stack.alloc_size + 1);
			}
		}

		switch (env->code[pc]) {
			case EDGE_LD_BYTE: {
				int offset;
				ST(env->stack, 1) = Private_do_push_byte(env->code[pc + 1], &env->code[pc + 2], &offset);
				env->stack.stack_pointer++;
				pc += 2 + offset;
				break;
			}
			case EDGE_LD_NULL: {
				ST(env->stack, 1) = Edge_create_null();
				env->stack.stack_pointer++;
				pc++;
				break;	
			}
			case EDGE_LD_STRING: {
				int offset;
				ST(env->stack, 1) = Edge_create_string(&env->code[pc + 1], &offset);
				env->stack.stack_pointer++;
				pc += 1 + offset;
				break;				
			}
			case EDGE_LD_VAR: {
				char *name = &env->code[pc + 1];
				ST(env->stack, 1) = Private_load_local_variable(env, name);
				env->stack.stack_pointer++;
				pc += 2 + strlen(name);
				break;
			}
			case EDGE_INIT: {
				Edge_BasicType type = env->code[pc + 1];
				char *identifier = &env->code[pc + 2];
				Private_init_local_variable(env, type, identifier);
				pc += 3 + strlen(identifier);
				break;
			}
			case EDGE_POP_BYTE: {
				printf("#pop_byte: %ld\n", GET_BIT(ST_INT64(env->stack, 0), ST_TYPE(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_POP_FLOAT: {
				printf("#pop_single: %f\n", ST_DOUBLE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_POP_STRING: {
				printf("#pop_string: %ls\n", get_visual_string(ST_STRING(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_MOV_VAR: {
				char *name = &env->code[pc + 1];
				Private_assign_local_variable(env, name, ST(env->stack, 0));
				env->stack.stack_pointer--;
				pc += 2 + strlen(name);
				break;
			}
			case EDGE_ADD_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) + ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_ADD_FLOAT: {
				ST_WRITE_DOUBLE(env->stack, -1, ST_DOUBLE(env->stack, -1) + ST_DOUBLE(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_ADD_STRING: {
				ST_STRING(env->stack, -1) = chain_string(ST(env->stack, -1), ST(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_SUB_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) - ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_MUL_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) * ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_DIV_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) / ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case EDGE_JUMP: {
				pc = env->code[pc + 1];
				break;
			}
			case EDGE_EXIT: {
				goto EXECUTE_END;
				break;
			}
			default: {
				if (env->wflag != EDGE_NOTHING) {
					DBG_panic(("Undefined bytecode %d in pc %d\n", env->code[pc], pc));
				} else {
					pc++;
				}
			}
		}
		Walle_reset_mark();
		Private_walle_marker(env);
		Walle_check_mem();
		/*for (; prev_stack > env->stack.stack_pointer && prev_stack >= 0; prev_stack--) {
			Private_dispose_value(&ST_i(env->stack, prev_stack));
		}*/
	}
	EXECUTE_END:;
	return;
}
