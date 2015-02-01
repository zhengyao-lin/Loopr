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
	(ST_TYPE((stack), (offset)) == LPR_SINGLE \
	? (stack).value[(stack).stack_pointer + (offset)]->u.single_value \
	: (stack).value[(stack).stack_pointer + (offset)]->u.double_value)
#define ST_STRING(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.string_value)
#define ST_OBJECT(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->u.object_value)

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
	((num) << (Loopr_Type_Info[LPR_INT64].size - Loopr_Type_Info[type].size) * 8 \
		   >> (Loopr_Type_Info[LPR_INT64].size - Loopr_Type_Info[type].size) * 8)

#define get_visual_string(src) ((src) ? (src)->u.string_value : NULL_VISUAL)
#define is_unsigned(type) \
	(type % 2 != 0 ? LPR_True : LPR_False)

Loopr_Char *
chain_string(Loopr_Value *str1, Loopr_Value *str2)
{
	int len;
	Loopr_Char *ret;
	Loopr_Char *right;
	Loopr_Char *left;

	right = get_visual_string(str1);
	left = get_visual_string(str2);

	len = Loopr_wcslen(right) + Loopr_wcslen(left);
	ret = MEM_malloc(sizeof(Loopr_Char) * (len + 1));

	Loopr_wcscpy(ret, right);
	Loopr_wcscat(ret, left);

	if (str1 && str1->u.string_value) {
		MEM_free(str1->u.string_value);
	}

	return ret;
}

Loopr_Value *
Private_do_push_byte(Loopr_BasicType basic_type, Loopr_Byte *code, int *offset)
{
	Loopr_Value *ret;

	ret = Loopr_alloc_value(basic_type);
	Loopr_byte_deserialize(&ret->u.int64_value,
						  code, Loopr_Type_Info[basic_type].size);
	*offset = Loopr_Type_Info[basic_type].size;

	return ret;
}

void
Private_init_local_variable(ExeEnvironment *env, Loopr_BasicType type, char *identifier)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (!strcmp(env->local_variable[i].identifier,
					identifier)) {
			if (env->wflag > LPR_JUST_PANIC) {
				DBG_panic(("Duplicated variable name \"%s\"\n", identifier));
			}
			return;
		}
	}

	env->local_variable = MEM_realloc(env->local_variable,
									  sizeof(LocalVariable) * (env->local_variable_count + 1));
	env->local_variable[env->local_variable_count].value = Loopr_get_init_value(type);
	env->local_variable[env->local_variable_count].identifier = MEM_strdup(identifier);
	env->local_variable_count++;

	return;
}

Loopr_Value *
Private_copy_variable(Loopr_Value *src)
{
	Loopr_Value *ret;

	if (src) {
		ret = Loopr_alloc_value(src->table->type);
		switch (src->table->type) {
			case LPR_STRING:
				if (src->u.string_value) {
					ret->u.string_value = MEM_malloc(sizeof(Loopr_Char) * (Loopr_wcslen(src->u.string_value) + 1));
					Loopr_wcscpy(ret->u.string_value, src->u.string_value);
				} else {
					ret->u.string_value = NULL;
				}
				break;
			default:
				ret->u = src->u;
				break;
		}
	} else {
		ret = NULL;
	}

	return ret;
}

void
Private_assign_local_variable(ExeEnvironment *env, char *name, Loopr_Value *value)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (!strcmp(env->local_variable[i].identifier,
					name)) {
			env->local_variable[i].value = Private_copy_variable(value);
			return;
		}
	}
	if (env->wflag != LPR_NOTHING) {
		DBG_panic(("Cannot find local variable \"%s\"\n", name));
	}

	return;
}

Loopr_Value *
Private_load_local_variable(ExeEnvironment *env, char *name)
{
	int i;
	for (i = 0; i < env->local_variable_count; i++) {
		if (!strcmp(env->local_variable[i].identifier,
					name)) {
			return Private_copy_variable(env->local_variable[i].value);
		}
	}
	if (env->wflag != LPR_NOTHING) {
		DBG_panic(("Cannot find local variable \"%s\"\n", name));
	}

	return;
}

void
Private_dispose_value(Loopr_Value **target)
{
	switch ((*target)->table->type) {
		case LPR_STRING:
			MEM_free((*target)->u.string_value);
			break;
	}
	MEM_free((*target)->table);
	MEM_free(*target);
	*target = NULL;

	return;
}

void
Private_expand_stack(Loopr_Stack *orig, int resize)
{
	orig->value = MEM_realloc(orig->value, sizeof(Loopr_Value *) * resize);
	orig->alloc_size = resize;

	return;
}

void
Private_mark_value(Loopr_Value *obj)
{
	if (!obj) {
		return;
	}

	obj->marked = LPR_True;
	switch (obj->table->type) {
		case LPR_OBJECT:
			Private_mark_value(obj->u.object_value);
	}

	return;
}

void
Private_walle_marker(ExeEnvironment *env)
{
	int i;

	for (i = 0; i < env->local_variable_count; i++) {
		Private_mark_value(env->local_variable[i].value);
	}

	for (i = 0; i <= env->stack.stack_pointer; i++) {
		Private_mark_value(env->stack.value[i]);
	}

	return;
}

void
Loopr_execute(ExeEnvironment *env)
{
	int prev_stack;
	int pc = env->entrance;

	for (; pc < env->code_length;) {

		printf("%4d:\t%-15ssp(%d)\n", pc, Loopr_Byte_Info[env->code[pc]].assembly_name, env->stack.stack_pointer);
		prev_stack = env->stack.stack_pointer;

		if (env->stack.stack_pointer < (Loopr_Byte_Info[env->code[pc]].need_stack - 1)
			&& env->wflag != LPR_NOTHING) {
			DBG_panic(("Too few stack for code \"%s\"\n", Loopr_Byte_Info[env->code[pc]].assembly_name));
		}

		if (env->stack.stack_pointer >= env->stack.alloc_size - 1) {
			if (env->wflag > LPR_JUST_PANIC) {
				DBG_panic(("Stack overflow\n"));
			} else {
				Private_expand_stack(&env->stack, env->stack.alloc_size + 1);
			}
		}

		switch (env->code[pc]) {
			case LPR_LD_BYTE: {
				int offset;
				ST(env->stack, 1) = Private_do_push_byte(env->code[pc + 1], &env->code[pc + 2], &offset);
				env->stack.stack_pointer++;
				pc += 2 + offset;
				break;
			}
			case LPR_LD_NULL: {
				ST(env->stack, 1) = Loopr_create_null();
				env->stack.stack_pointer++;
				pc++;
				break;	
			}
			case LPR_LD_STRING: {
				int offset;
				ST(env->stack, 1) = Loopr_create_string(&(env->code[pc + 1]), &offset);
				env->stack.stack_pointer++;
				pc += 1 + offset;
				break;				
			}
			case LPR_LD_LOC: {
				char *name = &env->code[pc + 1];
				ST(env->stack, 1) = Private_load_local_variable(env, name);
				env->stack.stack_pointer++;
				pc += 2 + strlen(name);
				break;
			}
			case LPR_INIT_LOC: {
				Loopr_BasicType type = env->code[pc + 1];
				char *identifier = &env->code[pc + 2];
				Private_init_local_variable(env, type, identifier);
				pc += 3 + strlen(identifier);
				break;
			}
			case LPR_BOXING: {
				ST(env->stack, 0) = Loopr_create_object(ST(env->stack, 0));
				pc++;
				break;
			}
			case LPR_UNBOXING: {
				ST(env->stack, 0) = (ST(env->stack, 0) ? ST_OBJECT(env->stack, 0) : NULL);
				pc++;
				break;
			}
			case LPR_POP_BYTE: {
				Loopr_BasicType type = ST_TYPE(env->stack, 0);
				if (is_unsigned(type)) {
					printf("#pop_%s: %lu\n", Loopr_Type_Info[type].assembly_name
										   , GET_BIT(ST_UINT64(env->stack, 0), type));
				} else {
					printf("#pop_%s: %ld\n", Loopr_Type_Info[type].assembly_name
										   , GET_BIT(ST_INT64(env->stack, 0), type));
				}
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_POP_FLOAT: {
				if (ST_TYPE(env->stack, 0) == LPR_SINGLE) {
					printf("#pop_single: %f\n", ST_SINGLE(env->stack, 0));
				} else {
					printf("#pop_double: %lf\n", ST_DOUBLE(env->stack, 0));
				}
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_POP_STRING: {
				printf("#pop_string: %ls\n", get_visual_string(ST(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_STORE_LOC: {
				char *name = &env->code[pc + 1];
				Private_assign_local_variable(env, name, ST(env->stack, 0));
				env->stack.stack_pointer--;
				pc += 2 + strlen(name);
				break;
			}
			case LPR_ADD_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) + ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_ADD_FLOAT: {
				ST_WRITE_DOUBLE(env->stack, -1, ST_DOUBLE(env->stack, -1) + ST_DOUBLE(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_ADD_STRING: {
				ST_STRING(env->stack, -1) = chain_string(ST(env->stack, -1), ST(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_SUB_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) - ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_MUL_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) * ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_DIV_BYTE: {
				ST_WRITE_INT64(env->stack, -1, ST_INT64(env->stack, -1) / ST_INT64(env->stack, 0));
				ST_TYPE(env->stack, -1) = TYPE_CMP(ST_TYPE(env->stack, -1),
												   ST_TYPE(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_JUMP: {
				Loopr_Int32 jpc;
				Loopr_byte_deserialize(&jpc, &env->code[pc + 1], sizeof(Loopr_Int32));
				pc = jpc;
				printf("#jump to: %d\n", pc);
				break;
			}
			case LPR_EXIT: {
				goto EXECUTE_END;
				break;
			}
			default: {
				if (env->wflag != LPR_NOTHING) {
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
