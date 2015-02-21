#include <stdio.h>
#include <string.h>
#include <math.h>
#include "LBS.h"
#include "DBG.h"
#include "UTL.h"
#include "MEM.h"
#include "SandBox_pri.h"

#define ST(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)])
#define ST_i(stack, p) \
	((stack).value[p])
#define ST_flag(stack, offset) \
	((stack).ref_flag[(stack).stack_pointer + (offset)])
#define ST_flag_i(stack, p) \
	((stack).ref_flag[p])


#define ST_TYPE(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)]->type)

#define ST_INTEGER(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)].int_value)
#define ST_FLOAT(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)].float_value)
#define ST_REF(stack, offset) \
	((stack).value[(stack).stack_pointer + (offset)].ref_value)

#define ST_WRITE_INTEGER(stack, offset, v) \
	((stack).value[(stack).stack_pointer + (offset)].int_value = v, \
	 (stack).ref_flag[(stack).stack_pointer + (offset)] = LPR_False)
#define ST_WRITE_FLOAT(stack, offset, v) \
	((stack).value[(stack).stack_pointer + (offset)].float_value = v, \
	 (stack).ref_flag[(stack).stack_pointer + (offset)] = LPR_False)
#define ST_WRITE_REF(stack, offset, v) \
	((stack).value[(stack).stack_pointer + (offset)].ref_value = v, \
	 (stack).ref_flag[(stack).stack_pointer + (offset)] = LPR_True)

#define TYPE_CMP(t1, t2) \
	((t1) >= (t2) ? (t1) : (t2))
#define GET_BIT(num, type) \
	((num) << (Loopr_Type_Info[LPR_INT64].size - Loopr_Type_Info[type].size) * 8 \
		   >> (Loopr_Type_Info[LPR_INT64].size - Loopr_Type_Info[type].size) * 8)
#define is_unsigned(type) \
	(type % 2 != 0 ? LPR_True : LPR_False)

static Loopr_Ref *
chain_string(Loopr_Ref *str1, Loopr_Ref *str2)
{
	int len;
	Loopr_Ref *ret;
	Loopr_Char *right;
	Loopr_Char *left;

	right = get_visual_string(str1);
	left = get_visual_string(str2);
	ret = Loopr_alloc_ref(LPR_STRING);

	len = Loopr_wcslen(right) + Loopr_wcslen(left);
	ret->u.string_value = MEM_malloc(sizeof(Loopr_Char) * (len + 1));

	Loopr_wcscpy(ret->u.string_value, right);
	Loopr_wcscat(ret->u.string_value, left);

	return ret;
}

static Loopr_Value
Private_do_push_byte(Loopr_BasicType basic_type, Loopr_Byte *code, int *offset)
{
	Loopr_Value ret_value = { 0 };

	Loopr_byte_deserialize(&ret_value.int_value,
						   code, Loopr_Type_Info[basic_type].size);
	*offset = Loopr_Type_Info[basic_type].size;

	return ret_value;
}

static void
Private_init_local_variable(ExeEnvironment *env, Loopr_BasicType type, int index)
{
#ifdef SENSLTIVE
	if (index < 0 && env->wflag != LPR_NOTHING) {
		DBG_panic(("init: Cannot find local variable\n"));
		return;
	} else if (index >= env->local_variable_map->count) {
		/*env->local_variable_map->variable = MEM_realloc(env->local_variable_map->variable,
									  	  sizeof(LocalVariable) * (env->local_variable_map->count + 1));
		env->local_variable_map->variable[index].identifier = NULL;
		env->local_variable_map->count++;*/
		DBG_panic(("init: Local variable overflow\n"));
		return;
	}
#endif

	env->local_variable_map->variable[index].value = Loopr_get_init_value(type);
	return;
}

static void
Private_assign_local_variable(ExeEnvironment *env, int index, Loopr_Value value, Loopr_Boolean ref_flag)
{
#ifdef SENSLTIVE
	if ((index < 0 || index >= env->local_variable_map->count)
		&& env->wflag != LPR_NOTHING) {
		DBG_panic(("stloc: Cannot find local variable\n"));
		return;
	}
#endif

	env->local_variable_map->variable[index].identifier = NULL;
	env->local_variable_map->variable[index].value = value;
	env->local_variable_map->variable[index].ref_flag = ref_flag;

	return;
}

static Loopr_Value
Private_load_local_variable(ExeEnvironment *env, int index, Loopr_Boolean *ref_flag)
{
#ifdef SENSLTIVE
	if ((index < 0 || index >= env->local_variable_map->count)
		&& env->wflag != LPR_NOTHING) {
		DBG_panic(("ldloc: Cannot find local variable\n"));
		return NULL;
	}
#endif

	*ref_flag = env->local_variable_map->variable[index].ref_flag;
	return env->local_variable_map->variable[index].value;
}

static void
Private_expand_stack(Loopr_Stack *stack, int add)
{
	int rest;

	rest = stack->alloc_size - stack->stack_pointer - 2; /* the two offset is for callinfo and overflow check */
	if (rest < add) {
		stack->value = MEM_realloc(stack->value, sizeof(Loopr_Value) * (stack->alloc_size + add - rest));
		stack->ref_flag = MEM_realloc(stack->ref_flag, sizeof(Loopr_Boolean) * (stack->alloc_size + add - rest));
		stack->alloc_size += add - rest;
	}

	return;
}

static void
Private_mark_value(Loopr_Ref *obj)
{
	if (!obj) {
		return;
	}

	obj->marked = Walle_get_alive_period();
	if (obj->type == LPR_OBJECT && obj->u.object_value.ref_flag) {
		Private_mark_value(obj->u.object_value.value.ref_value);
	} else if (obj->type == LPR_ARRAY) {
		int i;
		for (i = 0; i < obj->u.array_value.size; i++) {
			if (obj->u.array_value.ref_flag[i]) {
				Private_mark_value(obj->u.array_value.value[i].ref_value);
			}
		}
	}

	return;
}

static ExeEnvironment *current_top_level = NULL;
#define Private_set_top_level(env) (current_top_level = env)
#define Private_get_top_level() (current_top_level)

static void
Private_walle_marker(ExeEnvironment *env)
{
	int i;
	LocalVariableMap *pos;

	for (pos = env->local_variable_map; pos; pos = pos->prev) {
		for (i = 0; i < pos->count; i++) {
			if (pos->variable[i].ref_flag) {
				Private_mark_value(pos->variable[i].value.ref_value);
			}
		}
	}

	for (i = 0; i <= env->stack.stack_pointer; i++) {
		if (env->stack.ref_flag[i]) {
			Private_mark_value(env->stack.value[i].ref_value);
		}
	}

	return;
}

static ExeEnvironment *callee;
static ExeEnvironment *outer;
static int pri_i, pri_j;

static LocalVariableMap *
Private_init_arguments(int argc)
{
	LocalVariableMap *ret;

	ret = MEM_malloc(sizeof(LocalVariableMap));
	ret->count = argc;
	if (argc > 0) {
		ret->variable = MEM_malloc(sizeof(LocalVariable) * ret->count);
	} else {
		ret->variable = NULL;
	}
	ret->prev = NULL;

	return ret;
}

static void
Private_dispose_arguments(LocalVariableMap *map)
{
	if (map->variable) {
		MEM_free(map->variable);
	}
	MEM_free(map);
	return;
}

static void
Private_invoke_function(ExeEnvironment *env, int name_space, int index, int argc,
						int *pc_p, int *base_p)
{
	Loopr_Value ret_value;
	CallInfo *call_info;
	ExeEnvironment *callee;

	outer = Private_get_top_level()->sub_name_space[name_space];
	callee = outer->function[index];
	*pc_p += 2 + sizeof(Loopr_Int32) + sizeof(Loopr_Int32);

	if (callee->native_function) {/* is native function */
		env->stack.stack_pointer -= argc;
		ret_value = callee->native_function->callee(env, argc,
													&env->stack.value[env->stack.stack_pointer + 1]);
		ST(env->stack, 1) = ret_value;
		env->stack.stack_pointer++;
		return;
	}

	/* record info */
	call_info = malloc(sizeof(CallInfo));
	call_info->marked = 0;
	call_info->filler = 0;
	call_info->pc = *pc_p;
	call_info->caller = env->exe;

	Private_expand_stack(&env->stack, callee->stack.alloc_size);
	call_info->base = *base_p;
	call_info->stack_pointer = env->stack.stack_pointer - argc;
	call_info->local_list = env->local_variable_map;

	/* reset stack */
	env->stack.stack_pointer -= argc;
	*base_p = env->stack.stack_pointer + argc + 1;
	env->stack.stack_pointer = *base_p;

	/* invoke */
	env->local_variable_map = Private_init_arguments(callee->local_variable_map->count);
	env->local_variable_map->prev = call_info->local_list;

	/*for (pri_i = 0; pri_i < argc; pri_i++) {
		Private_assign_local_variable(env, pri_i, ST_i(env->stack, *base_p + pri_i), ST_flag_i(env->stack, *base_p + pri_i));
	}*/

	env->stack.value[*base_p].call_info = call_info;
	env->stack.ref_flag[*base_p] = LPR_False;

	env->exe = callee->exe;
	*pc_p = callee->exe->entrance;
	return;
}

static Loopr_Value
Private_do_return(ExeEnvironment *env, int *pc_p, int *base_p)
{
	Loopr_Value ret_value;
	Loopr_Boolean flag;
	CallInfo *call_info;

	call_info = ST_i(env->stack, *base_p).call_info;
	ret_value = ST(env->stack, 0);
	flag = ST_flag(env->stack, 0);

	env->stack.stack_pointer = call_info->stack_pointer;
	env->exe = call_info->caller;
	*pc_p = call_info->pc;
	*base_p = call_info->base;

	Private_dispose_arguments(env->local_variable_map);

	env->local_variable_map = call_info->local_list;
	ST_flag(env->stack, 1) = flag;

	free(call_info);

	return ret_value;
}

static char *
Private_convert_string_to_byte(Loopr_Char *str, char *controller)
{
	char ret[16];
	char *conv_str = NULL;

	conv_str = malloc(sizeof(char) * (Loopr_wcstombs_len(str) + 1));
	Loopr_wcstombs(str, conv_str);
	sscanf(conv_str, controller, ret);
	if (conv_str) {
		free(conv_str);
		MEM_free(str);
	}

	return ret;
}

static void
Private_convert(Loopr_BasicType from, Loopr_BasicType to, Loopr_Value *dest, Loopr_Boolean *ref_flag)
{
	int length;
	char buffer[CONV_STRING_BUFFER_SIZE];

	if (from == to) {
		return;
	}

	if (from == LPR_STRING) {
		memcpy(&dest->int_value,
			   Private_convert_string_to_byte(dest->ref_value->u.string_value,
											  Loopr_Type_Info[to].scan_controller),
			   Loopr_Type_Info[to].size);
		*ref_flag = LPR_False;
		return;
	}

	if (to == LPR_STRING) {
		if (from >= LPR_BOOLEAN && from <= LPR_UINT64) {
			sprintf(buffer, Loopr_Type_Info[from].scan_controller, dest->int_value);
		} else if (from == LPR_DOUBLE) {
			sprintf(buffer, Loopr_Type_Info[from].scan_controller, dest->float_value);
		}

		dest->ref_value = Loopr_alloc_ref(LPR_STRING);
		*ref_flag = LPR_True;
		dest->ref_value->u.string_value = Loopr_conv_string(buffer);
		return;
	}

	switch (to) {
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
			if (from <= LPR_UINT64) {
				dest->int_value = GET_BIT(dest->int_value, to);
			} else if (from == LPR_SINGLE) {
				dest->int_value = GET_BIT((Loopr_Int64)dest->float_value, to);
			} else if (from == LPR_DOUBLE) {
				dest->int_value = GET_BIT((Loopr_Int64)dest->float_value, to);
			}
			*ref_flag = LPR_False;
			break;
		case LPR_DOUBLE:
			if (from <= LPR_UINT64) {
				dest->float_value = (Loopr_Double)GET_BIT(dest->int_value, from);
			}
			*ref_flag = LPR_False;
			break;
		case LPR_OBJECT:
			DBG_panic(("Convert Object type: Use bx/unbx instead\n"));
			break;
		default:
			DBG_panic(("Unsupport convert type: %s to %s\n",
					   Loopr_Type_Info[from].assembly_name,
					   Loopr_Type_Info[to].assembly_name));
			break;
	}

	return;
}

#define LOWER_LEVEL ("|--\t")

static Loopr_Ref *
Private_create_one_dim_array(Loopr_Int32 size)
{
	Loopr_Ref *ret;

	ret = Loopr_alloc_ref(LPR_ARRAY);
	ret->u.array_value.size = size;
	if (size) {
		ret->u.array_value.value = MEM_malloc(sizeof(Loopr_Value) * size);
		ret->u.array_value.ref_flag = MEM_malloc(sizeof(Loopr_Boolean) * size);

		memset(&ret->u.array_value.value[0], NULL_VALUE, sizeof(Loopr_Value) * size);
		memset(&ret->u.array_value.ref_flag[0], NULL_VALUE, sizeof(Loopr_Boolean) * size);
	} else {
		ret->u.array_value.value = NULL;
		ret->u.array_value.ref_flag = NULL;
	}

	return ret;
}

static Loopr_Ref *
Private_create_array(ExeEnvironment *env, int dim)
{
	Loopr_Ref *ret;
	int size = ST_INTEGER(env->stack, -dim + 1);
	int i;

	if (dim == 1) {
		ret = Private_create_one_dim_array(size);
		return ret;
	}

	ret = Loopr_alloc_ref(LPR_ARRAY);
	ret->u.array_value.size = size;
	if (size) {
		ret->u.array_value.value = MEM_malloc(sizeof(Loopr_Value) * size);
		ret->u.array_value.ref_flag = MEM_malloc(sizeof(Loopr_Boolean) * size);
		for (i = 0; i < size; i++) {
			ret->u.array_value.value[i].ref_value = Private_create_array(env, dim - 1);
			ret->u.array_value.ref_flag[i] = LPR_True;
		}
	} else {
		ret->u.array_value.value = NULL;
		ret->u.array_value.ref_flag = NULL;
	}

	return ret;
}

static Loopr_Value
Private_get_array(Loopr_Ref *array, int index, Loopr_Boolean *ref_flag)
{
	Loopr_Value ret_value;

	if (array && array->type == LPR_ARRAY) {
		if (index < array->u.array_value.size) {
			*ref_flag = array->u.array_value.ref_flag[index];
			return array->u.array_value.value[index];
		} else {
			DBG_panic(("Array range error: The size of array is %d, but the given index is %d\n",
					   array->u.array_value.size,
					   index));
		}
	} else {
		DBG_panic(("Use array method to non-array value\n"));
	}
	return ret_value;
}

static void
Private_store_array(Loopr_Ref *array, int index, Loopr_Value value, Loopr_Boolean ref_flag)
{
	if (array && array->type == LPR_ARRAY) {
		if (index < array->u.array_value.size) {
			array->u.array_value.value[index] = value;
			array->u.array_value.ref_flag[index] = ref_flag;
			return;
		} else {
			DBG_panic(("Array range error: The size of array is %d, but the given index is %d\n",
					   array->u.array_value.size,
					   index));
		}
	} else {
		DBG_panic(("Use array method to non-array value\n"));
	}
	return;
}

Loopr_Value *
Loopr_execute(ExeEnvironment *env, Loopr_Boolean top_level)
{
	int arg1;
	int arg2;
	int base = 0;
	int pc = env->exe->entrance;
	Loopr_Value ret_value;

	if (top_level) {
		env->local_variable_map->variable = MEM_malloc(sizeof(LocalVariable) * env->local_variable_map->count);
		for (pri_i = 0; pri_i < env->local_variable_map->count; pri_i++) {
			env->local_variable_map->variable[pri_i].value.ref_value = Loopr_create_null();
		}
		env->stack.value = MEM_malloc(sizeof(Loopr_Value) * (env->stack.alloc_size + 1));
		env->stack.ref_flag = MEM_malloc(sizeof(Loopr_Boolean) * (env->stack.alloc_size + 1));
		Private_set_top_level(env);
	}
	for (; pc < env->exe->length;) {
		/*printf("%s%4d:\t%-15ssp(%d)\n", (!top_level ? LOWER_LEVEL : ""), pc,
			   Loopr_Byte_Info[env->exe->code[pc]].assembly_name,
			   env->stack.stack_pointer);*/

#ifdef SENSLTIVE
		if (env->stack.stack_pointer < (Loopr_Byte_Info[env->exe->code[pc]].need_stack - 1)
			&& env->wflag != LPR_NOTHING) {
			DBG_panic(("Too few stack for code \"%s\"\n", Loopr_Byte_Info[env->exe->code[pc]].assembly_name));
		}

		if (env->stack.stack_pointer >= env->stack.alloc_size - 2) {
			if (env->wflag > LPR_JUST_PANIC) {
				DBG_panic(("Stack overflow\n"));
			} else {
				Private_expand_stack(&env->stack, 1);
			}
		}
#endif

		switch (env->exe->code[pc]) {
			case LPR_LD_BYTE: {
				ST(env->stack, 1) = Private_do_push_byte(env->exe->code[pc + 1],
														 &env->exe->code[pc + 2],
														 &arg1);
				ST_flag(env->stack, 1) = LPR_False;
				env->stack.stack_pointer++;
				pc += 2 + arg1;
				break;
			}
			case LPR_LD_CONST: {
				ST_WRITE_INTEGER(env->stack, 1, env->exe->code[pc + 1]);
				env->stack.stack_pointer++;
				pc += 2;
				break;
			}
			case LPR_LD_NULL: {
				ST_WRITE_REF(env->stack, 1, Loopr_create_null());
				env->stack.stack_pointer++;
				pc++;
				break;
			}
			case LPR_LD_STRING: {
				ST_WRITE_REF(env->stack, 1,
							 Loopr_create_string(&(env->exe->code[pc + 1]), &arg1));
				env->stack.stack_pointer++;
				pc += 1 + arg1;
				break;
			}
			case LPR_LD_LOC: {
				Loopr_byte_deserialize(&arg1,
						  			   &env->exe->code[pc + 1], sizeof(Loopr_Int32));
				ST(env->stack, 1) = Private_load_local_variable(env, arg1,
																&ST_flag(env->stack, 1));
				env->stack.stack_pointer++;
				pc += 1 + sizeof(Loopr_Int32);
				break;
			}
			case LPR_LD_ARG: {
				ST(env->stack, 1) = ST_i(env->stack, base - env->exe->code[pc + 1] - 1);
				ST_flag(env->stack, 1) = ST_flag_i(env->stack, base - env->exe->code[pc + 1] - 1);
				env->stack.stack_pointer++;
				pc += 2;
				break;
			}
			case LPR_LD_ARRAY: {
				Loopr_byte_deserialize(&arg1,
						  			   &env->exe->code[pc + 1], sizeof(Loopr_Int32));
				ST(env->stack, 0) = Private_get_array(ST_REF(env->stack, 0), arg1,
													  &ST_flag(env->stack, 0));
				pc += 1 + sizeof(Loopr_Int32);
				break;
			}
			case LPR_CONVERT: {
				Private_convert(env->exe->code[pc + 1], env->exe->code[pc + 2],
								&ST(env->stack, 0), &ST_flag(env->stack, 0));
				pc += 3;
				break;
			}
			case LPR_BOXING: {
				ST_WRITE_REF(env->stack, 0,
							 Loopr_create_object(ST(env->stack, 0),
												 ST_flag(env->stack, 0)));
				pc++;
				break;
			}
			case LPR_UNBOXING: {
				ST(env->stack, 0) = ST_REF(env->stack, 0)->u.object_value.value;
				ST_flag(env->stack, 0) = ST_REF(env->stack, 0)->u.object_value.ref_flag;
				pc++;
				break;
			}
			case LPR_POP: {
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_POP_BYTE: {
				printf("#pop_byte: %ld\n", ST_INTEGER(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_POP_FLOAT: {
				printf("#pop_float: %lf\n", ST_FLOAT(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_POP_STRING: {
				printf("#pop_string: %ls\n", get_visual_string(ST_REF(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_STORE_LOC: {
				Loopr_byte_deserialize(&arg1,
						  			   &env->exe->code[pc + 1], sizeof(Loopr_Int32));
				Private_assign_local_variable(env, arg1, ST(env->stack, 0), ST_flag(env->stack, 0));
				env->stack.stack_pointer--;
				pc += 1 + sizeof(Loopr_Int32);
				break;
			}
			case LPR_STORE_ARRAY: {
				Loopr_byte_deserialize(&arg1,
						  			   &env->exe->code[pc + 1], sizeof(Loopr_Int32));
				Private_store_array(ST_REF(env->stack, -1), arg1, ST(env->stack, 0), ST_flag(env->stack, 0));
				env->stack.stack_pointer -= 2;
				pc += 1 + sizeof(Loopr_Int32);
				break;
			}
			case LPR_EQUAL_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   (ST_FLOAT(env->stack, -1) == ST_FLOAT(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_EQUAL: {
				ST_WRITE_INTEGER(env->stack, -1,
								 (ST_INTEGER(env->stack, -1) == ST_INTEGER(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_NOT_EQUAL_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   (ST_FLOAT(env->stack, -1) != ST_FLOAT(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_NOT_EQUAL: {
				ST_WRITE_INTEGER(env->stack, -1,
								 (ST_INTEGER(env->stack, -1) != ST_INTEGER(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_GREATER_THAN_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   (ST_FLOAT(env->stack, -1) > ST_FLOAT(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_GREATER_THAN: {
				ST_WRITE_INTEGER(env->stack, -1,
								 (ST_INTEGER(env->stack, -1) > ST_INTEGER(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_LESS_THAN_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   (ST_FLOAT(env->stack, -1) < ST_FLOAT(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_LESS_THAN: {
				ST_WRITE_INTEGER(env->stack, -1,
								 (ST_INTEGER(env->stack, -1) < ST_INTEGER(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_LESS_OR_EQUAL_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   (ST_FLOAT(env->stack, -1) <= ST_FLOAT(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_LESS_OR_EQUAL: {
				ST_WRITE_INTEGER(env->stack, -1,
								(ST_INTEGER(env->stack, -1) <= ST_INTEGER(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_GREATER_OR_EQUAL_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   (ST_FLOAT(env->stack, -1) >= ST_FLOAT(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_GREATER_OR_EQUAL: {
				ST_WRITE_INTEGER(env->stack, -1,
								(ST_INTEGER(env->stack, -1) >= ST_INTEGER(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_BRANCH: {
				if (env->exe->code[pc + 1] == (ST_INTEGER(env->stack, 0) == env->exe->code[pc + 2])) {
					Loopr_byte_deserialize(&pc, &env->exe->code[pc + 3], sizeof(Loopr_Int32));
				} else {
					pc += 3 + sizeof(Loopr_Int32);
				}
				env->stack.stack_pointer--;
				break;
			}
			case LPR_DUPLICATE: {
				ST(env->stack, 1) = ST(env->stack, -env->exe->code[pc + 1]);
				ST_flag(env->stack, 1) = ST_flag(env->stack, -env->exe->code[pc + 1]);
				env->stack.stack_pointer++;
				pc += 2;
				break;
			}
			case LPR_ADD_BYTE: {
				ST_WRITE_INTEGER(env->stack, -1,
								 ST_INTEGER(env->stack, -1) + ST_INTEGER(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_ADD_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   ST_FLOAT(env->stack, -1) + ST_FLOAT(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_ADD_STRING: {
				ST_WRITE_REF(env->stack, -1,
							 chain_string(ST_REF(env->stack, -1), ST_REF(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_SUB_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   ST_FLOAT(env->stack, -1) - ST_FLOAT(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_SUB_BYTE: {
				ST_WRITE_INTEGER(env->stack, -1,
								 ST_INTEGER(env->stack, -1) - ST_INTEGER(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_MUL_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   ST_FLOAT(env->stack, -1) * ST_FLOAT(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_MUL_BYTE: {
				ST_WRITE_INTEGER(env->stack, -1,
								 ST_INTEGER(env->stack, -1) * ST_INTEGER(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_DIV_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   ST_FLOAT(env->stack, -1) / ST_FLOAT(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_DIV_BYTE: {
				ST_WRITE_INTEGER(env->stack, -1,
								 ST_INTEGER(env->stack, -1) / ST_INTEGER(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_MOD_FLOAT: {
				ST_WRITE_FLOAT(env->stack, -1,
							   fmod(ST_FLOAT(env->stack, -1), ST_FLOAT(env->stack, 0)));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_MOD_BYTE: {
				ST_WRITE_INTEGER(env->stack, -1,
								 ST_INTEGER(env->stack, -1) % ST_INTEGER(env->stack, 0));
				env->stack.stack_pointer--;
				pc++;
				break;
			}
			case LPR_INC_FLOAT: {
				ST_FLOAT(env->stack, 0)++;
				pc++;
				break;
			}
			case LPR_INC: {
				ST_INTEGER(env->stack, 0)++;
				pc++;
				break;
			}
			case LPR_DEC_FLOAT: {
				ST_FLOAT(env->stack, 0)--;
				pc++;
				break;
			}
			case LPR_DEC: {
				ST_INTEGER(env->stack, 0)--;
				pc++;
				break;
			}
			case LPR_MINUS_FLOAT: {
				ST_FLOAT(env->stack, 0) = -ST_FLOAT(env->stack, 0);
				pc++;
				break;
			}
			case LPR_MINUS: {
				ST_INTEGER(env->stack, 0) = -ST_INTEGER(env->stack, 0);
				pc++;
				break;
			}
			case LPR_INVOKE: {
				Loopr_byte_deserialize(&arg1,
						  			   &env->exe->code[pc + 1], sizeof(Loopr_Int32));
				Loopr_byte_deserialize(&arg2,
						  			   &env->exe->code[pc + 1 + sizeof(Loopr_Int32)], sizeof(Loopr_Int32));
				Private_invoke_function(env, arg1, arg2,
										env->exe->code[pc + 1 + sizeof(Loopr_Int32) + sizeof(Loopr_Int32)],
										&pc, &base);
				break;
			}
			case LPR_NEW_ARRAY: {
				arg1 = env->exe->code[pc + 1]; /* dim */
				ST_REF(env->stack, -arg1 + 1) = Private_create_array(env, arg1);
				ST_flag(env->stack, -arg1 + 1) = LPR_True;

				env->stack.stack_pointer -= arg1 - 1;
				pc += 2;
				break;
			}
			case LPR_GOTO: {
				Loopr_byte_deserialize(&pc, &env->exe->code[pc + 1], sizeof(Loopr_Int32));
				/*printf("#goto: %d\n", pc);*/
				break;
			}
			case LPR_RETURN: {
				ret_value = Private_do_return(env, &pc, &base);
				ST(env->stack, 1) = ret_value;
				/* the flag had been set in Private_do_return(...) */
				env->stack.stack_pointer++;
				break;
			}
			case LPR_NOP: {
				pc++;
				break;
			}
			default: {
				if (env->wflag != LPR_NOTHING) {
					DBG_panic(("Undefined bytecode %d in pc %d\n", env->exe->code[pc], pc));
				} else {
					pc++;
				}
			}
		}
		if (Walle_get_alloc_size() >= Walle_get_threshold()) {
			Walle_update_alive_period();
			Private_walle_marker(Private_get_top_level());
			Walle_check_mem();
		}
	}
	EXECUTE_END:

	return NULL;
}
