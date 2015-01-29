#include <stdio.h>
#include <time.h>
#include "SandBox_pri.h"
#include "MEM.h"
#define WALLE_COLLECT_THRESHOLD (sizeof(Loopr_Value) * 64)

static void (*__walle_marker)(void);
static Loopr_Int64 __threshold = WALLE_COLLECT_THRESHOLD;
static Loopr_Int64 __allocd_size = 0;
static Loopr_Value *__walle_header = NULL;

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
	Loopr_Value *pos;

	header = Walle_get_header();
	for (pos = header; pos && pos->next; pos = pos->next) {
	}

	if (!pos) {
		Walle_set_header(v);
	} else {
		pos->next = v;
		v->prev = pos;
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
Walle_dispose_value(Loopr_Value **target)
{
	switch ((*target)->table->type) {
		case LPR_STRING:
			MEM_free((*target)->u.string_value);
			break;
	}
	MEM_free((*target)->table);

	if ((*target)->prev && (*target)->next) {
		(*target)->prev->next = (*target)->next;
		(*target)->next->prev = (*target)->prev;
	} else if ((*target)->prev) {
		(*target)->prev->next = NULL;
	} else if ((*target)->next) {
		(*target)->next->prev = NULL;
		Walle_set_header((*target)->next);
	}

	MEM_free(*target);
	*target = NULL;

	return;
}

void
Walle_gcollect()
{
	Loopr_Value *header;
	Loopr_Value *pos;
	Loopr_Value *tmp;

	header = Walle_get_header();
	if (Walle_get_marker()) {
		(*Walle_get_marker())();
	}

	/*printf("start collecting\n");
	printf("alloc'd: %d\n", Walle_get_alloc_size());*/
	for (pos = header; pos;) {
		if (pos->marked != LPR_True) {
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
