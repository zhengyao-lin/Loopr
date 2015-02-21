#include <stdio.h>
#include <time.h>
#include "LBS.h"
#include "MEM.h"
#include "SandBox_pri.h"

#define WALLE_COLLECT_THRESHOLD (sizeof(Loopr_Ref) * 2048)

static void (*__walle_marker)(void);
static Loopr_Int64 __threshold = WALLE_COLLECT_THRESHOLD;
static Loopr_Int64 __allocd_size = 0;
static Loopr_Ref *__walle_header = NULL;
static Loopr_Ref *__walle_tail = NULL;
static int __alive_period = 1;

void
Walle_update_alive_period()
{
	__alive_period++;
	return;
}

int
Walle_get_alive_period()
{
	return __alive_period;
}

void
Walle_set_header(Loopr_Ref *v)
{
	__walle_header = v;
	return;
}

Loopr_Ref *
Walle_get_header()
{
	return __walle_header;
}

void
Walle_add_alloc_size(Loopr_Int64 add)
{
	__allocd_size += add;
	return;
}

Loopr_Int64
Walle_get_alloc_size()
{
	return __allocd_size;
}

void
Walle_add_threshold(Loopr_Int64 add)
{
	__threshold += add;
	return;
}

Loopr_Int64
Walle_get_threshold()
{
	return __threshold;
}

void
Walle_set_marker(Walle_Marker marker)
{
	__walle_marker = marker;
	return;
}

Walle_Marker
Walle_get_marker()
{
	return __walle_marker;
}

void
Walle_add_object(Loopr_Ref *v)
{
	if (!__walle_header) {
		__walle_header = __walle_tail = v;
	} else {
		__walle_tail->next = v;
		v->prev = __walle_tail;
		__walle_tail = v;
	}

	__allocd_size += sizeof(Loopr_Ref);

	return;
}

void
Walle_dispose_value(Loopr_Ref **target)
{
	Loopr_Ref *tmp;

	if ((*target)->type == LPR_STRING) {
		MEM_free((*target)->u.string_value);
	} else if ((*target)->type == LPR_ARRAY) {
		MEM_free((*target)->u.array_value.value);
		MEM_free((*target)->u.array_value.ref_flag);
	}

	if ((*target)->prev) {
		(*target)->prev->next = (*target)->next;
	} else {
		__walle_header = (*target)->next;
	}

	if ((*target)->next) {
		(*target)->next->prev = (*target)->prev;
	} else {
		__walle_tail = (*target)->next;
	}

	tmp = (*target)->next;
	MEM_free(*target);
	*target = tmp;
	__allocd_size -= sizeof(Loopr_Ref);

	return;
}


void
Walle_gcollect()
{
	Loopr_Ref *pos;

	for (pos = __walle_header; pos;) {
		if (pos->marked != __alive_period) {
			Walle_dispose_value(&pos);
			continue;
		}
		pos = pos->next;
	}

	return;
}

void
Walle_check_mem()
{
	if (__allocd_size >= __threshold) {
		Walle_gcollect();
		if (__allocd_size >= __threshold) {
			__threshold += __allocd_size + WALLE_COLLECT_THRESHOLD;
		}
	}

	return;
}

void
Walle_dispose_exe_container(ExeContainer *exe)
{
	MEM_free(exe->code);
	MEM_free(exe);
	return;
}

void
Walle_dispose_environment(ExeEnvironment *env)
{
	int i;

	if (!env) {
		return;
	}

	Walle_dispose_exe_container(env->exe);
	if (env->stack.value) {
		MEM_free(env->stack.value);
		MEM_free(env->stack.ref_flag);
	}
	if (env->local_variable_map->variable) {
		MEM_free(env->local_variable_map->variable);
	}
	MEM_free(env->local_variable_map);

	for (i = 0; i < env->sub_name_space_count; i++) {
		if (env->sub_name_space[i] != env) {
			Walle_dispose_environment(env->sub_name_space[i]);
		}
	}
	MEM_free(env->sub_name_space);

	for (i = 0; i < env->function_count; i++) {
		Walle_dispose_environment(env->function[i]);

	}
	MEM_free(env->function);

	MEM_free(env);

	return;
}

void
Walle_dispose_byte_container(ByteContainer *env, Loopr_Boolean flag_clean_code)
{
	int i;

	if (!env) {
		return;
	}

	if (env->name) {
		MEM_free(env->name);
	}

	for (i = 0; i < env->sub_name_space_count; i++) {
		if (env->sub_name_space[i] != env) {
			Walle_dispose_byte_container(env->sub_name_space[i], flag_clean_code);
		}
	}
	MEM_free(env->sub_name_space);

	for (i = 0; i < env->function_count; i++) {
		Walle_dispose_byte_container(env->function[i], flag_clean_code);
	}
	MEM_free(env->function);

	if (flag_clean_code) {
		MEM_free(env->code);
	}

	MEM_free(env);

	return;
}
