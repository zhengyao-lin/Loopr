#include <stdio.h>
#include <time.h>
#include "SandBox_pri.h"
#include "MEM.h"
#define WALLE_COLLECT_THRESHOLD (sizeof(Loopr_Value) * 1024)

static void (*__walle_marker)(void);
static Loopr_Int64 __threshold = WALLE_COLLECT_THRESHOLD;
static Loopr_Int64 __allocd_size = 0;
static Loopr_Value *__walle_header = NULL;
static Loopr_Value *__walle_tail = NULL;
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
Walle_set_header(Loopr_Value *v)
{
	__walle_header = v;
	return;
}

Loopr_Value *
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
Walle_add_object(Loopr_Value *v)
{
	Loopr_Value *header;
	Loopr_Value *tail;
	Loopr_Value *pos;

	header = Walle_get_header();
	tail = __walle_tail;

	if (!tail) {
		Walle_set_header(v);
		__walle_tail = v;
	} else {
		tail->next = v;
		v->prev = tail;
		__walle_tail = v;
	}

	v->next = NULL;
	Walle_add_alloc_size(sizeof(Loopr_Value));

	return;
}

void
Walle_reset_mark()
{
	Loopr_Value *header;
	Loopr_Value *pos;

	header = Walle_get_header();
	for (pos = header; pos; pos = pos->next) {
		pos->marked = LPR_False;
	}

	return;
}

void
Walle_mark_all()
{
	Loopr_Value *header;
	Loopr_Value *pos;

	header = Walle_get_header();
	for (pos = header; pos; pos = pos->next) {
		pos->marked = LPR_True;
	}

	return;
}

void
Walle_dispose_value(Loopr_Value **target)
{
	if ((*target)->table->type == LPR_STRING) {
		MEM_free((*target)->u.string_value);
	}

	MEM_free((*target)->table);

	if ((*target)->prev) {
		(*target)->prev->next = (*target)->next;
	}
	if ((*target)->next) {
		(*target)->next->prev = (*target)->prev;
	}
	if (!((*target)->prev || (*target)->next)) {
		Walle_set_header((*target)->next);
	}

	MEM_free(*target);

	return;
}

void
Walle_gcollect()
{
	static Loopr_Value *header;
	static Loopr_Value *pos;
	static Loopr_Value *tmp;

	header = Walle_get_header();
	if (Walle_get_marker()) {
		(*Walle_get_marker())();
	}

	for (pos = header; pos;) {
		if (pos->marked != Walle_get_alive_period()) {
			tmp = pos->next;
			Walle_dispose_value(&pos);
			Walle_add_alloc_size(-sizeof(Loopr_Value));
			pos = tmp;
			continue;
		}
		pos = pos->next;
	}

	return;
}

void
Walle_check_mem()
{
	if (Walle_get_alloc_size() >= Walle_get_threshold()) {
		Walle_gcollect();
		if (Walle_get_alloc_size() >= Walle_get_threshold()) {
			Walle_add_threshold(WALLE_COLLECT_THRESHOLD);
		}
	}

	return;
}

void
Walle_dispose_environment(ExeEnvironment *env)
{
	int i;

	MEM_free(env->code);
	MEM_free(env->stack.value);
	for (i = 0; i < env->local_variable_count; i++) {
		if (env->local_variable[i].identifier) {
			MEM_free(env->local_variable[i].identifier);
		}
	}
	if (env->local_variable) {
		MEM_free(env->local_variable);
	}

	for (i = 0; i < env->function_count; i++) {
		Walle_dispose_environment(env->function[i]);
	}
	MEM_free(env->function);

	MEM_free(env);

	return;
}

void
Walle_dispose_function(ExeEnvironment *env)
{
	int i;

	MEM_free(env->stack.value);
	for (i = 0; i < env->local_variable_count; i++) {
		if (env->local_variable[i].identifier) {
			MEM_free(env->local_variable[i].identifier);
		}
	}
	if (env->local_variable) {
		MEM_free(env->local_variable);
	}

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

	if (env->name) {
		MEM_free(env->name);
	}

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
